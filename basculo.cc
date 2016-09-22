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



int main(int argc, char **argv) {

  typedef typename std::vector< types::silicio<Nstrip> > detector_t;
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

  for(int isili=0;isili<Nsili;++isili)
    sili.push_back(types::silicio<Nstrip>(spede[0],srms[0],status[0]));

  read_pede<detector_t>(sili,p["pedestal"].GetString());
  read_status<Nsili,Nstrip>(status,p["status"].GetString());


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

    for(int isili=0;isili<Nsili;++isili)
      hplot(110+isili);
    
    int pause;
    std::cin >> pause;


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
  int iev=1,ierr,id=1, run_number=1;


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
    std::cout << s << std::endl;

    apri_tupla_(s.c_str(),nmax);
    //    apri_tupla(s,nmax);

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


      for(int isili=0;isili < Nsili;++isili) {

        // zeros dead strips
        std::transform(sili[isili].value.begin(),sili[isili].value.end(),
                       sili[isili].spede.begin(),
                       sili[isili].value.begin(),
                       [&](float arg1, bool arg2) { 
                         return (arg2 > 0? arg1 : 0.0); });
        
        // subtracts pede
        std::transform(sili[isili].value.begin(),sili[isili].value.end(),
                       sili[isili].spede.begin(),
                       gap.begin(),
                       std::minus<float>());
        
        {
          const int Nasic=3;
          std::array<float,Nasic> cm,n;
          for(int iasic=0;iasic<Nasic;++iasic) {
            
            for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
              if( gap[istrip] < p["soglia_cm"].GetFloat()*sili[isili].srms[istrip] && !sili[isili].status[istrip] ) {
                cm[iasic] += gap[istrip];
                n[iasic]++;
              }
            cm[iasic]/=n[iasic];
            for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
              if(!sili[isili].status[istrip])
                sili[isili].value[istrip] = gap[istrip] - cm[iasic];
            
          }
        }

        sili[isili].process();
        // std::cout << (*sili[isili].ph_max) << "\t" 
        //           << (*sili[isili].pull  ) << "\t" 
        //           <<   sili[isili].eta     << "\t" 
        //           << std::endl;
        hfill( 100+isili,(*sili[isili].ph_max),1.);
        hfill( 110+isili,(*sili[isili].pull),1.);


        std::vector<algo::cluster_data> c = algo::clusterize(sili[isili],cut[isili]);
        hf1(180+isili,c.size()); // numero di cluster

        if(c.size() > 0 ) {
          
          
          // for ( auto& cc : c ) {
          //   algo::process_cluster<types::silicio<Nstrip> >(cc, sili[isili],cut_low[isili],pitch);
          //   cc.fill_histo(isili,pitch);
          //   //            if(cc.end - cc.begin == 2)
          //   cc.fill_histo_hit(isili,pitch);
          // }
        
        }
      } // Nsili
      
    }

    
    
    
    
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






