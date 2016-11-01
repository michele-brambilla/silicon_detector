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
const int Nsili=4;
const int Nsili_raw=5;

const int offset=4;
const int n_eta_bin = 100;


void prepara_histo(const int);


int main(int argc, char **argv) {

  int istat;
  int iev=1,ierr,id=1, run_number=1;

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
  

  types::pedestal<Nsili_raw,Nstrip> spede;
  types::rms<Nsili_raw,Nstrip> srms;
  spede(p["pedestal"].GetString());
  srms(p["pedestal"].GetString());

  //////////////////////////////
  // status
  types::status<Nsili_raw,Nstrip> status;
  //  status.read(p["status"].GetString());
  //st.set(spede);
  
  

  for(int i =0;i<Nstrip;++i) {
    status[0][i] = 0;
    std::cout << spede[0][i] << "\t" << status[0][i] << std::endl;
  }

  exit(0);


  std::vector< types::silicio<Nstrip> > sili;
  sili.push_back(types::silicio<Nstrip>(spede[0],srms[0],status[0]));
  sili.push_back(types::silicio<Nstrip>(spede[1],srms[1],status[1]));
  sili.push_back(types::silicio<Nstrip>(spede[3],srms[3],status[3]));
  sili.push_back(types::silicio<Nstrip>(spede[4],srms[4],status[4]));


  /////////////////////////
  // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str());

  prepara_histo(Nsili);


  std::array< std::vector<float>, Nsili> feta;
  std::ifstream feta_in(p["feta_file"].GetString());
  if ( feta_in.good() ) {
    std::string line;
    float x;
    int isili = 0;
    while (std::getline(feta_in, line)) {
      std::istringstream iss(line);
      std::vector<float> s;
      while( iss >> x ) s.push_back(x);
      feta[isili] = s;
      isili++;
    }
  }
  feta_in.close();
  
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

      //      std::cout << "evento: " << iev << std::endl;
      if(is_led_event()) continue;

      get_raw_data1_(&sili[0].value[0], &offset);
      get_raw_data2_(&sili[1].value[0], &offset);
      get_raw_data5_(&sili[2].value[0], &offset);
      get_raw_data6_(&sili[3].value[0], &offset);

      float pitch = p["pitch"].GetFloat();
      std::vector<float> cut;
      std::vector<float> cut_low;
      for(int i=0;i<Nsili;++i) {
        cut.push_back( p["cut"][i].GetFloat() );
        cut_low.push_back( p["cut_low"][i].GetFloat() );
      }
      std::vector<float> cm(3);
      if( (cut.size() != Nsili) ||
          (cut_low.size() != Nsili) )
        throw std::runtime_error("Numero valori cut sbagliati in input.js");
      

      // std::vector<float> o2(sili[2].value);
      for(int i=0;i<Nsili;++i) {
        subtract_pedestal(sili[i]);
      }

      // std::vector<float> i2(sili[2].value);
      for(int i=0;i<Nsili;++i) {
        for( int a = 0;a<3;++a) {
          cm[a] = compute_cm(sili[i],128*a,128*(a+1),cut_low[i]);
          subtract_commonmode(sili[i],128*a,128*(a+1),cm[a]);
        }
      }

      // for(int i=0;i<384/4;++i)
        // std::cout << o2[i] << "\t"
        //           << i2[i] << "\t"
        //           << sili[2].value[i] << "\t"
        //           << std::endl;
        // std::cout << sili[0].value[i] << "\t"
        //           << sili[1].value[i] << "\t"
        //           << sili[2].value[i] << "\t"
        //           << sili[3].value[i] << "\t"
        //           << std::endl;
      // continue;
      
      for(int i=0;i<Nsili;++i){      

        sili[i].process();
        std::vector<algo::cluster_data> c = algo::clusterize(sili[i],cut[i]);
        hf1(180+i,c.size()); // numero di cluster

        if(c.size() > 0 ) {
          
          hf1( 100+i,(*sili[i].ph_max));
          hf1( 110+i,(*sili[i].pull));
          
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


  // std::ofstream feta_out(p["feta_file"].GetString());
  // if ( feta_out.good() ) {
  //   for(int isili=0;isili<Nsili;++isili) {
  //     std::vector<float> se;
  //     hunpak_ (140+isili,&se[0],'',1)
  //     std::partial_sum (feta[isili].begin(), feta[isili].end(), se.begin());
  //     for( auto& v : se )
  //       feta_out << v;
  //     feta_out << "\n";
  //   }
  // }
  // feta_out.close();

  
  
  finalize_();
  return 0;
}




void prepara_histo(const int N) {

  hbook1(99,"dummy",100,0.,1000.);
  for(int i=0;i<N;++i) {
    // basics
    hbook1(100+i,"ph max",100,0.,1000.);
    hbook1(110+i,"pull",100,0.,250.);

    // ph vs strip
    hbook2(500+i,"ph max strip",100,0.,1000.,384,0,384);
    
    // charge distribution
    hbook1(120+i,"pull_L",100,.0,250.);
    hbook1(130+i,"pull_R",100,.0,250.);
    hbook1(140+i,"eta",n_eta_bin,-1,1.);

    // cluster
    hbook1(150+i,"amplitude",100,0.,1000.);
    hbook1(160+i,"snr",100,0.,500.);
    hbook1(170+i,"nr_strip",5,0.,5.);
    
    hbook1(180+i,"nr_cluster",6,-.5,5.5);

    hbook1(190+i,"x_cm",1000,0.,20000.);
    hbook1(200+i,"interstrip_cm",50,0.,50.);
    hbook1(210+i,"x_pos",1000,0.,20000.);
    hbook1(220+i,"insterstrip_pos",50,0.,50.);
    
  }
  
}
