#include<iostream>
#include<fstream>
#include<sstream>

#include<iterator>
#include<vector>
#include<iomanip>

#include "wrapper.hpp"
#include "input_parser.hpp"
#include "analisi.hpp"

extern std::vector<common_mode> common;


void write_file(int id, std::string name, detector_t& detector) {
  std:: ofstream of;
  of.open(name);
  for(int ibin=0;ibin < get_histogram(id).n;++ibin) {
    for(int isili=0;isili<detector.size();++isili) {
      of << get_histogram(id+isili).v[ibin] << "\t";
    }
    of << "\n";
  }
  of.close(); 
}

void get_data(detector_t&, int);

template<typename T>
bool leggi_pede_rms(T&,const char*);

template<typename T>
bool leggi_status(T&,const char*);


static const int Nstrip=384;
static const int Nmodules=5;
std::string input_file(const rapidjson::Document&, const int);


float get_random_number(float min=0.,float max=1000.) { return rand()/(float)RAND_MAX*(max-min)+min; }


int main(int argc, char **argv) {
  static const int offset=4;

  rapidjson::Document p = parse(argc,argv);

  int istat;
  int iev=1,ierr,id=1, run_number=1;

  int output_id=2;
  int tuple_id=40;

  // /////////////////////////
  // // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str(),&output_id);

  {
    // int id=99;
    // std::string name;
    // int nbin;
    // float min;
    // float max;
    // float zero;
    // hbook1_(&id,&name[0],&nbin,&min,&max,&zero,1);
    // for(int isili=0;isili<Nmodules;++isili) {
    //   id=100+isili;
    //   name="ph";
    //   nbin=500;
    //   min=0.0f;
    //   max=3000.0f;
    //   zero=0.0f;
    //   hbook1_(&id,&name[0],&nbin,&min,&max,&zero,1);
    //   id=110+isili;
    //   name="pull";
    //   nbin=200;
    //   min=0.0f;
    //   max=500.0f;
    //   hbook1_(&id,&name[0],&nbin,&min,&max,&zero,1);

    // }
    
    for(int isili=0;isili<Nmodules;++isili) {
      hbook1(100+isili,"ph",200,0.,2000.,1,1);
      hbook1(110+isili,"pull",100,0.,500.,1,1);
      hbook1(120+isili,"pullL",100,0.,500.,1,1);
      hbook1(130+isili,"pullR",100,0.,500.,1,1);
      hbook1(140+isili,"eta",100,-1.,1.,1,1);
    }
  }

  std::vector< silicio<float> > detector;
  for(int nsili=0;nsili<Nmodules;++nsili)
    detector.push_back(silicio<float>(Nstrip));

  std::vector<float> cut;
  for (auto& v : p["cut"].GetArray())
    cut.push_back(v.GetFloat());
  if(cut.size() != detector.size() ) {
    throw std::runtime_error("Not enough cut values");
  }
  for(int isili=0;isili<detector.size();++isili)
    detector[isili].cut=cut[isili];

  leggi_pede_rms<std::vector< silicio<float> > >(detector,
                                                 p["pedestal"].GetString());
  leggi_status<std::vector< silicio<float> > >(detector,
                                               p["status"].GetString());
  

  for(int isili=0;isili<detector.size();++isili)
    common.push_back(common_mode(detector[isili].spede,detector[isili].srms));

  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito
  do {
    int nmax=-1;
    std::string s=input_file(p,run_number);

    if(!std::ifstream(s)) {
      std::cerr << "File " << s << " mancante." << std::endl;
      continue;
    }
    
    std::cout << s << std::endl;
    apri_tupla_(s.c_str(),nmax,tuple_id);
    if(nmax == -1) 
      return -1;
    
    std::cout << "\n"
              << s << "\t"
              << "max events = " << nmax << std::endl;
    
    float strip;

    for(int iev=1;iev<nmax;++iev) {
      id=1;
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;

      int led_value = -1000;
      led_event_(&led_value);
      if( led_value < 0 ) {
        std::cout << iev << " is LED - " << led_value << std::endl;
        continue;
      }

      get_data(detector,offset);

      // for( auto& x : detector[0].data)
      //   std::cout << x << std::endl;
      // std::cin >> id;
     
      analisi_dati(detector,p);
 
    }
    chiudi_tupla_(tuple_id);
    
  } while ( ++run_number <= p["nfilemax"].GetInt() );
  
  write_file(100,"ph.dat",detector);
  write_file(110,"pull.dat",detector);
  write_file(120,"pullL.dat",detector);
  write_file(130,"pullR.dat",detector);
  write_file(140,"eta.dat",detector);


  finalize_(&output_id);

  return 0;
}



std::string input_file(const rapidjson::Document& p, const int run_number) {

  std::ostringstream ss;
  ss << std::setw(5) << std::setfill('0') << run_number;
  std::string s2(ss.str());

  std::string s =  std::string(p["input_directory"].GetString()) + "/run"+std::to_string(p["first"].GetInt())+"_"+s2+".hbook";
  std::cout << s << std::endl;
  s.resize(s.size());

  return s;
}







template<typename T>
bool leggi_pede_rms(T& detector, const char* filename) {

  float dummy0,dummy1,value,noise;

  std::ifstream is(filename);
  if(!is.good() )
    throw std::runtime_error("Impossibile aprire il file di pede\n");
  std::cout << "Reading pedestal: " << filename << "\n" << std::endl;
  
  for(int isili=0; isili<detector.size(); ++isili) {
    for(int istrip=0; istrip<detector[isili].spede.size(); ++istrip) {
      is >> dummy0 
         >> dummy1 
         >> detector[isili].spede[istrip] 
         >> detector[isili].srms[istrip];
      // std::cout << isili << "\t" << istrip 
      //           << "\t" << detector[isili].spede[istrip] 
      //           << "\t" << detector[isili].srms[istrip] 
      //           << "\n"; 
      if( is.eof() ) {
        std::cerr << "Impossibile leggere il file di pede\n";
        return false;
      }
    }
  }
  return true;
}

template<typename T>
bool leggi_status(T& detector, const char* filename) {

  std::ifstream is(filename);
  int value;

  if(!is.good() )
    throw std::runtime_error("Impossibile aprire il file di pede\n");
  std::cout << "Reading status: " << filename << "\n" << std::endl;
  
  for(int istrip=0; istrip<detector[0].spede.size(); ++istrip) {
    for(int isili=0; isili<detector.size(); ++isili) {
      is >> value;
      detector[isili].status[istrip]=static_cast<bool>(value);
      if( is.eof() )
        return false;
    }
  }
  return true;
}


void get_data(detector_t& detector, int offset) {
  get_raw_data1_(&detector[0].data[0], &offset);
  get_raw_data2_(&detector[1].data[0], &offset);
  get_raw_data3_(&detector[2].data[0], &offset);
  get_raw_data5_(&detector[3].data[0], &offset);
  get_raw_data6_(&detector[4].data[0], &offset);
}
