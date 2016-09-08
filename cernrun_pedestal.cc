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
  
  //////////////////////////////
  // status
  types::status<Nsili_raw,Nstrip> status(Nstrip);
  types::pedestal<Nsili_raw,Nstrip> pede(Nstrip);
  types::rms<Nsili_raw,Nstrip> rms(Nstrip);
  types::pedestal<Nsili_raw,Nstrip> spede(Nstrip);
  types::rms<Nsili_raw,Nstrip> srms(Nstrip);
  
  std::vector< types::silicio<Nstrip> > sili;
  sili.push_back(types::silicio<Nstrip>(pede[0],rms[0],status[0]));
  sili.push_back(types::silicio<Nstrip>(pede[1],rms[1],status[1]));
  sili.push_back(types::silicio<Nstrip>(pede[3],rms[3],status[3]));
  sili.push_back(types::silicio<Nstrip>(pede[4],rms[4],status[4]));
  
  /////////////////////////
  // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str());

  prepara_histo(Nsili);

  int nmax=-1;
  
  std::string s =  std::string(p["input_directory"].GetString()) + "/run"+std::to_string(p["first"].GetInt())+"_pede.hbook";
  
  if(!std::ifstream(s)) {
    std::cerr << "File " << s << " mancante." << std::endl;
  }
    
  s.resize(s.size());
  std::cout << s << std::endl;

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

    get_raw_data1_(&sili[0].value[0], &offset);
    get_raw_data2_(&sili[1].value[1], &offset);
    get_raw_data5_(&sili[2].value[2], &offset);
    get_raw_data6_(&sili[3].value[3], &offset);

    
    //////////////////
    // riempie profile histo
    for(int i=0;i<4;++i)
      for(int istrip=0;istrip<384;++istrip) {
        hfill(100+i,istrip,sili[i].value[istrip]);
      }
  }

  chiudi_tupla_();

  ///////////////
  // riempie vettori pede e rms
  // con contenuto profile histo
  for(int i=0;i<Nsili;++i) {
    hunpak(100+i,&pede[i][0]);    
    hunpke(100+i,&rms[i][0]);
  }


  for(int isili=0;isili<Nsili;++isili) {
    float avg = std::accumulate(rms[isili].begin(),rms[isili].end(),0.)/rms[isili].size();
    float avg2 = std::inner_product(rms[isili].begin(),rms[isili].end(),rms[isili].begin(),0.)/rms[isili].size();
    float sd = sqrt(avg2 - avg*avg);
    std::cout << "avg = " << avg
              << "sd = " << sd
              << std::endl;
    
    
    for(int istrip=0;istrip<384;++istrip) {
      if( (rms[isili][istrip] < (avg - p["dead_threshold"].GetFloat()*sd) ) ||
          (rms[isili][istrip] > (avg + p["dead_threshold"].GetFloat()*sd) ) ) {
        status[isili][istrip] = true;
      }
    }
    
  }

  std::ofstream os(p["status"].GetString());
  for(int istrip=0;istrip<384;++istrip) {
    for(int isili=0;isili<Nsili;++isili)
      os << status[isili][istrip] << "\t";
    os << std::endl;
  }
  os.close();
  
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

    get_raw_data1_(&sili[0].value[0], &offset);
    get_raw_data2_(&sili[1].value[1], &offset);
    get_raw_data5_(&sili[2].value[2], &offset);
    get_raw_data6_(&sili[3].value[3], &offset);

    

    std::vector<float> cm(3),n(3);
    std::vector<float> t(Nstrip);
    //////////////////
    // riempie profile histo pede-sottratto
    for(int isili=0;isili<Nsili;++isili) {
      
      for(int istrip=0;istrip<384;++istrip)
        t[istrip] = sili[isili].value[istrip] - pede[isili][istrip];

      for(int iasic=0;iasic<3;++iasic) {
        cm[iasic] = std::accumulate(t.begin()+128*iasic,t.begin()+128*(iasic+1),0.);
        n[iasic]  = std::accumulate(status[isili].begin()+128*iasic,status[isili].begin()+128*(iasic+1),0);
        cm[iasic] /= (128-n[iasic]);
      }

      for(int istrip=0;istrip<384;++istrip)
        if(!status[isili][istrip])
          sili[isili].value[istrip] -= cm[istrip/128];
      

      for(int istrip=0;istrip<384;++istrip)
        if(!status[isili][istrip]) {
          hfill(200+isili,istrip,sili[isili].value[istrip]);
        }
    }
  }
  
  ///////////////
  // riempie vettori pede e rms
  // con contenuto profile histo
  for(int i=0;i<Nsili;++i) {
    hunpak(200+i,&spede[i][0]);    
    hunpke(200+i,&srms[i][0]);
  }


  os.open(p["pedestal"].GetString());
  for(int istrip=0;istrip<384;++istrip) {
    for(int isili=0;isili<Nsili;++isili)
      os << pede[isili][istrip]  << "\t"
         << rms[isili][istrip]   << "\t"
         << spede[isili][istrip] << "\t"
         << srms[isili][istrip] << "\n";
  }
  os.close();
  
  
  finalize_();
  return 0;
}




void prepara_histo(const int N) {


  for(int i=0;i<N;++i) {
    // no-sub
    hbprof(100+i,"pedestal",Nstrip,0.,384.);
    hbprof(200+i,"pedestal-cmsub",Nstrip,0.,384.);
  }
  
}
