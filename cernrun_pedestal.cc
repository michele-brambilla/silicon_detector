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
void set_status(const std::vector<float>&,
                const std::vector<float>&,
                float,
                std::vector<bool>&);

int main(int argc, char **argv) {

  int istat;
  int iev=1,ierr,id=1, run_number=1;

  rapidjson::Document p;
  rapidjson::ParseResult ok;
  if(argc>1)
    ok =p.Parse(read_config_file(argv[1]).c_str());
  else
    ok =p.Parse(read_config_file("input_pede.js").c_str());
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
    get_raw_data2_(&sili[1].value[0], &offset);
    get_raw_data5_(&sili[2].value[0], &offset);
    get_raw_data6_(&sili[3].value[0], &offset);

    //////////////////
    // riempie profile histo
    for(int isili=0;isili<Nsili;++isili) {
      for(int istrip=0;istrip<384;++istrip) {
        hfill(100+isili,istrip,sili[isili].value[istrip]);
      }
    }
    
  }

  ///////////////
  // riempie vettori pede e rms
  // con contenuto profile histo
  for(int isili=0;isili<Nsili;++isili) {
    hunpak(100+isili,&pede[isili][0]);    
    hunpke(100+isili,&rms[isili][0]);
  }

  chiudi_tupla_();

  for(int isili=0;isili<Nsili;++isili) {
    set_status(pede[isili],rms[isili],p["dead_threshold"].GetFloat(),status[isili]);
  }

  // for(int isili=0;isili<Nsili;++isili) {
  //   float avg = std::accumulate(rms[isili].begin(),rms[isili].end(),0.)/rms[isili].size();
  //   float avg2 = std::inner_product(rms[isili].begin(),rms[isili].end(),rms[isili].begin(),0.)/rms[isili].size();
  //   float sd = sqrt(avg2 - avg*avg);
  //   std::cout << "avg = " << avg
  //             << "sd = " << sd
  //             << std::endl;
    
    
  //   for(int istrip=0;istrip<384;++istrip) {
  //     if( (rms[isili][istrip] < (avg - p["dead_threshold"].GetFloat()*sd) ) ||
  //         (rms[isili][istrip] > (avg + p["dead_threshold"].GetFloat()*sd) ) ) {
  //       status[isili][istrip] = true;
  //     }
  //   }
    


  std::ofstream os(p["status"].GetString());
  for(int istrip=0;istrip<384;++istrip) {
    for(int isili=0;isili<Nsili;++isili)
      os << status[isili][istrip] << "\t";
    os << std::endl;
  }
  os.close();

  prepara_histo(Nsili);
  
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
    get_raw_data2_(&sili[1].value[0], &offset);
    get_raw_data5_(&sili[2].value[0], &offset);
    get_raw_data6_(&sili[3].value[0], &offset);

    for(int isili=0;isili<Nsili;++isili)  {
    // spede = value - pede
      std::transform(sili[isili].value.begin(),sili[isili].value.end(),
                     pede[isili].begin(),
                     spede[isili].begin(),
                   std::minus<float>());    

      for(int istrip=0;istrip<Nstrip;++istrip)
        if( status[isili][istrip] )  {
          //          sili[isili].value[istrip] = 0;
          spede[isili][istrip] = 0;
        }

      std::vector<float> cm(3),n(3);
      for(int iasic=0;iasic<3;++iasic) {
        int first = 128*iasic;
        int last = 128*(iasic+1);
        
        n[iasic] = 128 - std::accumulate( status[isili].begin() + first,
                                          status[isili].begin() + last,
                                          0.);
        cm[iasic] = std::accumulate( spede[isili].begin() + first,
                                     spede[isili].begin() + last,
                                     0.)/n[iasic];

        std::transform(sili[isili].begin() + first,
                       sili[isili].begin() + last, 
                       sili[isili].begin() + first,
                       std::bind2nd(std::minus<float>(), cm[iasic]) );              
      }

      for(int istrip=0;istrip<Nstrip;++istrip) {
        if( !status[isili][istrip] ) 
          hfill(100+isili,istrip,sili[isili].value[istrip]);
        else
          hfill(100+isili,istrip,0);
      }      
      
      
      // for( auto x : cm )
      //   std::cout << x << "\t";
      // std::cout << std::endl;
      // for( auto x : n )
      //   std::cout << x << "\t";
      // std::cout << std::endl;

    }

    // //////////////////
    // // riempie profile histo
    // for(int isili=0;isili<Nsili;++isili) {
    //   for(int istrip=0;istrip<Nstrip;++istrip) {
    //     hfill(200+isili,istrip,sili[isili].value[istrip]);
    //   }
    // }
    

  }

  chiudi_tupla_();
  
  ///////////////
  // riempie vettori pede e rms
  // con contenuto profile histo
  for(int i=0;i<Nsili;++i) {
    hunpak(100+i,&spede[i][0]);    
    hunpke(100+i,&srms[i][0]);
  }


  os.open(p["pedestal"].GetString());
  for(int isili=0;isili<Nsili;++isili) {
    for(int istrip=0;istrip<Nstrip;++istrip) {
      os << std::setw(10) << pede[isili][istrip]  << "\t"
         << std::setw(10) << rms[isili][istrip]   << "\t"
         << std::setw(10) << spede[isili][istrip] << "\t"
         << std::setw(10) << srms[isili][istrip]  << "\n";
    }
  }
  os.close();
  
  
  finalize_();
  return 0;
}




void prepara_histo(const int N) {

  hbprof(99,"pedestal",Nstrip,0.,384.);
  for(int i=0;i<N;++i) {
    // no-sub
    hbprof(100+i,"pedestal",Nstrip,0.,384.);
    hbprof(200+i,"pedestal-cmsub",Nstrip,0.,384.);
  }
  
}



void set_status(const std::vector<float>& val,
                const std::vector<float>& rms,
                float cut,
                std::vector<bool>& result) {

  for(int iasic=0;iasic<3;++iasic) {
    int first = 128*iasic;
    int last  = 128*(iasic+1);
    float avg = std::accumulate(val.begin()+first,
                                val.begin()+last,
                                0.)/128.;
    float avg2 = std::inner_product(val.begin()+first,
                                    val.begin()+last,
                                    val.begin()+first,
                                    0.)/128.;
    float sd = sqrt(avg2 - avg*avg);
    
    float snr=0, snr2=0,t;
    for(int istrip=first;istrip<last;++istrip) {
      t = val[istrip]/(rms[istrip]);
      snr += t/128;
      snr2 += t*t/128;
    }
    
    float sd_snr = std::sqrt(std::abs(snr*snr-snr2));
    
    // std::cout << "avg = " << avg
    //           << "sd = " << sd
    //           << "\t";
    
    // std::cout << "snr = " << snr
    //           << "sd_snr = " << sd_snr
    //           << std::endl;

    for(int istrip=first;istrip<last;++istrip) {
      if( ( val[istrip]/(rms[istrip]) > snr*1.3 ) ||
          ( val[istrip]/(rms[istrip]) < snr*.7  ) ||
          ( val[istrip] > avg*1.2               ) ||
         ( val[istrip] < avg*.8 )              ) {
        result[istrip] = true;
        // std::cout << avg*.8 << " " << val[istrip] << " " << avg*1.2 << "\t"
        //           << snr*.7 << " " << val[istrip]/rms[istrip] << " " << snr*1.3 << "\n";
      }
    }
  }
  std::cout << std::endl;
  
}


