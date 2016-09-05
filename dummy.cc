#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<sstream>
#include<iomanip>


#include "status.hpp"
#include "pede_rms.hpp"
#include "silicio.hpp"
#include "algo.hpp"
//#include "cluster.hpp"

/////////
// JSON input parser
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/error.h>

const int Nstrip=384;
const int Nsili=3;
const int Nsili_raw=5;

const int offset=4;

void prepara_histo(const int);

std::ifstream open_config_file(const char* s) {
  std::ifstream in;
  try {
    in.open(s);
    in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  }
  catch (std::ifstream::failure e) {
    std::cout << "Exception opening/reading file: " << e.what() << std::endl;
  }
  return in;
}

std::string read_config_file(const char* s) {
  std::ifstream in = open_config_file(s);
  std::string config,buf;
  while(!in.eof()) {
     std::getline(in, buf,'\t');
     config += buf;
  }
  in.close();
  return config;
}


int main(int argc, char **argv) {

  int istat;
  int iev=1,ierr,id=1, run_number=1;

  ////////////////
  // Leggo le informazioni del file di input
  // uparam::Param p;
  // if(argc>1)
  //   p.read(argv[1]);
  // else
  //   p.read("input");

  
  rapidjson::Document p;
  rapidjson::ParseResult ok =p.Parse(read_config_file("input.js").c_str());
  if( !ok ) {
    throw std::runtime_error("Errore: file di configurazione non valido");
  }

  std:: cout << "nfilemax\t" << p["nfilemax"].GetInt() << std::endl;
  
  //////////////////////////////
  // status
  types::status<Nsili_raw,Nstrip> st;
  st(p["status"].GetString());

  types::pedestal<Nsili_raw,Nstrip> spede;
  types::rms<Nsili_raw,Nstrip> srms;
  spede(p["pedestal"].GetString());
  srms(p["pedestal"].GetString());

  std::vector< types::silicio<Nstrip> > sili;
  sili.push_back(types::silicio<Nstrip>(spede[0],srms[0],st[0]));
  sili.push_back(types::silicio<Nstrip>(spede[2],srms[2],st[2]));
  sili.push_back(types::silicio<Nstrip>(spede[4],srms[4],st[4]));


  std::cout << ( std::string(p["output_directory"].GetString())+"/"+
                 std::string(p["output"].GetString()) )
            << std::endl;
  
  /////////////////////////
  // Inizializzo il file di output
   // init_( ( std::string(p["output_directory"].GetString())+"/"+
   //                std::string(p["output"].GetString()) ) .c_str());

  //  prepara_histo(3);
  
  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito
  do {
    int nmax=-1;

    std::ostringstream ss;
    ss << std::setw(5) << std::setfill('0') << run_number;
    std::string s2(ss.str());
    
    std::string s = std::string(p["input_directory"].GetString()) + "/run"+std::to_string(p["first"].GetInt())+"_"+s2+".hbook";
    std::cout << s << std::endl;
    if(!std::ifstream(s)) {
      std::cerr << "File " << s << " mancante." << std::endl;
      continue;
    }
    
    std::cout << s << std::endl;
    s.resize(s.size());
    
    //    apri_tupla_(s.c_str(),nmax);
    // if(nmax == -1) 
    //   return -1;
    
    std::cout << std::endl;
    std::cout << s << std::endl;
    std::cout << "max events = " << nmax << std::endl;
    
    // for(int iev=1;iev<nmax;++iev) {
    //   hgnt_(&id,&iev,&ierr);
    //   if(ierr != 0) 
    //     return ierr;

    //   std::cout << "evento: " << iev << std::endl;
    //   if(is_led_event()) continue;

    //   get_raw_data_(&sili[0].value[0],
    //                 &sili[1].value[0],
    //                 &sili[2].value[0],
    //                 &offset);

    //   float pitch = 50; // um
    //   std::vector<float> cut = {10.,10.,10.};
    //   std::vector<float> cut_low = {5.,5.,5.};

    //   std::vector<float> cm;

      
    //   for(int i=0;i<Nsili;++i){
    //     cm = sili[i].pre_process(cut[i]);
    //     sili[i].process();

    //     hf2(12,cm[1],cm[2]);
    //     hf2(13,cm[1],cm[3]);
    //     hf2(23,cm[2],cm[3]);
        
    //     std::vector<algo::cluster_data> c = algo::clusterize(sili[i],cut[i]);
    //     hf1(180+i,c.size()); // numero di cluster

    //     if(c.size() > 0 ) {
          
    //       hf1( 100+i,(*sili[i].ph_max));
    //       hf1( 110+i,(*sili[i].pull));
          
    //       for ( auto& cc : c ) {
    //         algo::process_cluster<types::silicio<Nstrip> >(cc, sili[i],cut_low[i],pitch);
    //         cc.fill_histo(i,pitch);
    //         //            if(cc.end - cc.begin == 2)
    //           cc.fill_histo_hit(i,pitch);
            
            
    //       }
        
    //     }
    //   } // Nsili

    // }

    // chiudi_tupla_();
    
    // std::cout << run_number << "\t" <<  atoi(p["nfilemax"].c_str()) << "\t" << istat << std::endl; 
  } while ( ++run_number <= p["nfilemax"].GetInt() ); //&& istat==0
  
  //  finalize_();
  return 0;
}




