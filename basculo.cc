#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<iomanip>

#include "status.hpp"
#include "pede_rms.hpp"
#include "silicio.hpp"
#include "algo.hpp"
#include "cluster.hpp"

/////////
// JSON input parser
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>


const int Nsili_raw=5;
const int offset=4;

const int Nstrip=384;
const int Nsili=4;


typedef typename std::vector< types::silicio<Nstrip> > detector_t;

void read_pede(detector_t&, const std::string&);
void read_status(types::status<Nsili,Nstrip>&, const std::string&);

void pre_process(types::silicio<Nstrip>&, const float&);

int main(int argc, char **argv) {

  detector_t sili;
  types::pedestal<Nsili,Nstrip> spede;
  types::rms<Nsili,Nstrip> srms;
  types::status<Nsili,Nstrip> status;
  std::array< std::vector<float>, Nsili > feta;
  std::array<float,Nstrip> gap;

  rapidjson::Document p;
  rapidjson::ParseResult ok;
  if(argc>1)
    ok =p.Parse(read_config_file(argv[1]).c_str());
  else
    ok =p.Parse(read_config_file("input.js").c_str());
  if( !ok ) {
    throw std::runtime_error("Errore: file di configurazione non valido");
  }

  sili.push_back(types::silicio<Nstrip>(spede[0],srms[0],status[0]));
  sili.push_back(types::silicio<Nstrip>(spede[1],srms[1],status[1]));
  sili.push_back(types::silicio<Nstrip>(spede[2],srms[2],status[2]));
  sili.push_back(types::silicio<Nstrip>(spede[3],srms[3],status[3]));

  read_pede(sili,p["pedestal"].GetString());
  read_status(status,p["status"].GetString());


  float pitch = p["pitch"].GetFloat();
  std::vector<float> cut;
  std::vector<float> cut_low;
  for(int i=0;i<Nsili;++i) {
    cut.push_back( p["cut"][i].GetFloat() );
    cut_low.push_back( p["cut_low"][i].GetFloat() );
  }
  if( (cut.size() != Nsili) ||
      (cut_low.size() != Nsili) )
    throw std::runtime_error("Numero valori cut sbagliati in input.js");


  /////////////////////////
  // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str());

  prepara_histo(Nsili);
  
  

  // std::ifstream feta_in(p["feta_file"].GetString());
  // if ( feta_in.good() ) {
  //   std::string line;
  //   float x;
  //   int isili = 0;
  //   while (std::getline(feta_in, line)) {
  //     std::istringstream iss(line);
  //     std::vector<float> s;
  //     while( iss >> x ) s.push_back(x);
  //     feta[isili] = s;
  //     isili++;
  //   }
  // }
  // feta_in.close();

  
  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito

  int istat;
  int iev=1,ierr,id=1, run_number=0;


  do {
    int nmax=-1;

    std::ostringstream ss;
    ss << std::setw(5) << std::setfill('0') << run_number;
    std::string s2(ss.str());

    std::string s =  std::string(p["input_directory"].GetString()) + "/run"+std::to_string(p["first"].GetInt())+"_"+s2+".hbook";
    
    if(!std::ifstream(s)) {
      std::cerr << "File " << s << " mancante." << std::endl;
      continue;
    }
    
    s.resize(s.size());
    std::cerr << s << std::endl;

    apri_tupla_(s.c_str(),nmax);
    //    apri_tupla(s,nmax);

    if(nmax == -1) 
      return -1;
    
    std::cerr << std::endl;
    std::cerr << s << std::endl;
    std::cerr << "max events = " << nmax << std::endl;
    
    for(int iev=1;iev<nmax;++iev) {
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;

      //      std::cout << "evento: " << iev << std::endl;
      if(is_led_event()) continue;

      get_raw_data1_(&sili[0].value[0], &offset);
      get_raw_data2_(&sili[1].value[0], &offset);
      get_raw_data5_(&sili[2].value[0], &offset);
      get_raw_data6_(&sili[3].value[0], &offset);


      for(int isili=0;isili < Nsili;++isili) {

        pre_process(sili[isili],p["soglia_cm"].GetFloat());

        std::vector<float>::iterator pull = std::max_element(sili[isili].snr.begin(), sili[isili].snr.end());
        int ipull = std::distance(sili[isili].snr.begin(), pull);
        
        if( (*pull) > cut[isili] ) {
          hfill( 100+isili, *std::max_element(sili[isili].value.begin(),
                                              sili[isili].value.end()) ,1.);
          hfill( 110+isili, *pull, 1.);
        }
        

        // std::cout << "sili nr = " << isili << "\tcut = " << cut[isili] << std::endl;
        std::vector<algo::cluster_data> c = algo::clusterize(sili[isili],cut[isili]);
        // std::cout << "sili nr = " << isili << "\tnr cluster = "  << c.size() << std::endl;

        hf1(180+isili,c.size()); // numero di cluster

        // if(c.size() > 0 ) {
          
          
        //   // for ( auto& cc : c ) {
        //   //   algo::process_cluster<types::silicio<Nstrip> >(cc, sili[isili],cut_low[isili],pitch);
        //   //   cc.fill_histo(isili,pitch);
        //   //   //            if(cc.end - cc.begin == 2)
        //   //   cc.fill_histo_hit(isili,pitch);
        //   // }
        
        // }
      } // Nsili

      // int pause;
      // std::cin >> pause;
      // continue;
      
      
    }

    for(int isili=0;isili<Nsili;++isili)
      hplot(180+isili);
    
    
    // if(run_number % 50 == 0) {
    //   for(int isili=0;isili<Nsili;++isili)
    //     hplot(110+isili);
    // }
    
    chiudi_tupla_();
    
  //   //    std::cout << run_number << "\t" <<  atoi(p["nfilemax"].c_str()) << "\t" << istat << std::endl; 
  } while ( ( ++run_number <= p["nfilemax"].GetInt() ) ); //&& istat==0


  // // std::ofstream feta_out(p["feta_file"].GetString());
  // // if ( feta_out.good() ) {
  // //   for(int isili=0;isili<Nsili;++isili) {
  // //     std::vector<float> se;
  // //     hunpak_ (140+isili,&se[0],'',1)
  // //     std::partial_sum (feta[isili].begin(), feta[isili].end(), se.begin());
  // //     for( auto& v : se )
  // //       feta_out << v;
  // //     feta_out << "\n";
  // //   }
  // // }
  // // feta_out.close();

  
  finalize_();
  return 0;
}






void read_pede(detector_t& sili, const std::string& s) {

  std::ifstream is;
  is.open(s);
  int c=0,value,dummy;
  if( !is.good() )
    throw std::runtime_error("Impossibile aprere il file di stato");
  
  std::string line;  
  while(is.good() && (c/Nstrip < Nsili) ) {
    std::getline(is,line);
    std::istringstream ss(line);
    std::vector <float> record;
    while (ss) {
      std::string item;

      if (!std::getline( ss, item, '\t' )) break;
      record.push_back(std::stof(item));
    }
    sili[c/Nstrip].spede[c%Nstrip] = record[2];
    sili[c/Nstrip].srms[c%Nstrip]  = record[3];
    c++;
  }
  
  is.close();
}


void read_status(types::status<Nsili,Nstrip>& status, const std::string& s) {

  std::ifstream is(s);
  int istrip=0;

  if( !is.good() )
    throw std::runtime_error("Impossibile aprere il file di stato");

  std::string line;
  while(is.good() && (istrip < Nstrip) ) {
    std::getline(is,line);
    std::istringstream ss(line);
    std::vector <float> record;
    while (ss) {
      std::string item;
      if (!std::getline( ss, item, '\t' )) break;
      record.push_back(std::stoi(item));
    }
    for(int isili=0;isili<Nsili;++isili)
      status[isili][istrip] = record[isili];
    istrip++;
  }
  is.close();
}



void pre_process(types::silicio<Nstrip>& sili, const float& soglia) {
  std::array<float,Nstrip> gap;

  // zeros dead strips
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.spede.begin(),
                 sili.value.begin(),
                 [&](float arg1, bool arg2) { 
                   return (arg2 > 0? arg1 : 0.0); });


  // subtracts pede
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.spede.begin(),
                 gap.begin(),
                 std::minus<float>());
        
  {
    const int Nasic=3;
    std::array<float,Nasic> cm,n;
    for(int iasic=0;iasic<Nasic;++iasic) {
            
      for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
        if( gap[istrip] <soglia*sili.srms[istrip] && !sili.status[istrip] ) {
          cm[iasic] += gap[istrip];
          n[iasic]++;
        }
      cm[iasic]/=n[iasic];
      for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
        if(!sili.status[istrip])
          sili.value[istrip] = gap[istrip] - cm[iasic];
            
    }
  }

  // snr
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.srms.begin(),
                 sili.snr.begin(),
                 [&](float n, float d) { 
                   return (d > 0? n/d : 0.0); });

} // pre_process
