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


const int Nstrip=384;
const int Nsili=3;
const int Nsili_raw=5;

const int offset=4;

void prepara_histo(const int);


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
  rapidjson::ParseResult ok;
  if(argc>1)
    ok =p.Parse(read_config_file(argv[1]).c_str());
  else
    ok =p.Parse(read_config_file("input.js").c_str());
  
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

  /////////////////////////
  // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str());

  prepara_histo(3);

  // int ijk = 12;
  // std::cout << hexist_(&ijk) << std::endl;


  
  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito
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
    
    std::cout << s << std::endl;
    s.resize(s.size());

    apri_tupla_(s.c_str(),nmax);
    if(nmax == -1) 
      return -1;
    
    std::cout << std::endl;
    std::cout << s << std::endl;
    std::cout << "max events = " << nmax << std::endl;
    
    for(int iev=1;iev<nmax;++iev) {
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;

      std::cout << "evento: " << iev << std::endl;
      if(is_led_event()) continue;

      get_raw_data_(&sili[0].value[0],
                    &sili[1].value[0],
                    &sili[2].value[0],
                    &offset);

      float pitch = p["pitch"].GetFloat();
      std::vector<float> cut;
      std::vector<float> cut_low;
      for(int i=0;i<Nsili;++i) {
        cut.push_back( p["cut"][i].GetFloat() );
        cut_low.push_back( p["cut_low"][i].GetFloat() );
      }
      std::vector<float> cm;
      

      for(int i=0;i<Nsili;++i){
        cm = sili[i].pre_process(p["soglia_cm"].GetFloat());

        for(auto& c:cm)
          std::cout << c << "\t";
        std::cout << "\n";
        std::cin >> i;
          
        sili[i].process();
        hf2(12,cm[1],cm[2]);
        hf2(13,cm[1],cm[3]);
        hf2(23,cm[2],cm[3]);
        
        std::vector<algo::cluster_data> c = algo::clusterize(sili[i],cut[i]);
        hf1(180+i,c.size()); // numero di cluster

        if(c.size() > 0 ) {
          
          hf1( 100+i,(*sili[i].ph_max));
          hf1( 110+i,(*sili[i].pull));
          hf2( 500+i,(*sili[i].ph_max),std::distance(sili[i].ph_max,sili[i].begin()));
          
          for ( auto& cc : c ) {
            algo::process_cluster<types::silicio<Nstrip> >(cc, sili[i],cut_low[i],pitch);
            cc.fill_histo(i,pitch);
            //            if(cc.end - cc.begin == 2)
              cc.fill_histo_hit(i,pitch);
            
            
          }
        
        }
      } // Nsili

    }

    chiudi_tupla_();
    
    //    std::cout << run_number << "\t" <<  atoi(p["nfilemax"].c_str()) << "\t" << istat << std::endl; 
  } while ( ( ++run_number <= p["nfilemax"].GetInt() ) ); //&& istat==0
  
  finalize_();
  return 0;
}




void prepara_histo(const int N) {

  // per farne il plot, exec color#color
  // hi/pl 12(13,23) col 
  hbook2(12,"cm_correlation_asic12",100,0.1,6.,50,0.1,6.);
  hbook2(13,"cm_correlation_asic13",100,0.1,6.,50,0.1,6.);
  hbook2(23,"cm_correlation_asic23",100,0.1,6.,50,0.1,6.);

  for(int i=0;i<N;++i) {
    // basics
    hbook1(100+i,"ph max",1000,0.,500.);
    hbook1(110+i,"pull",100,0.,250.);

    // ph vs strip
    hbook2(500+i,"ph max strip",1000,0.,500.,384,0,384);
    
    // charge distribution
    hbook1(120+i,"pull_L",100,.0,250.);
    hbook1(130+i,"pull_R",100,.0,250.);
    hbook1(140+i,"eta",100,-1,1.);

    // cluster
    hbook1(150+i,"amplitude",100,0.,1000.);
    hbook1(160+i,"snr",100,0.,500.);
    hbook1(170+i,"nr_strip",5,0.,5.);
    
    hbook1(180+i,"nr_cluster",5,0.,5.);

    hbook1(190+i,"x_cm",1000,0.,20000.);
    hbook1(200+i,"interstrip_cm",1000,0.,50.);
    hbook1(210+i,"x_pos",1000,0.,20000.);
    hbook1(220+i,"insterstrip_pos",1000,0.,50.);
    
  }
  
}
