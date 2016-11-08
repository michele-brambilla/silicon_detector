#include<iostream>
#include<fstream>
#include<sstream>

#include<iterator>
#include<vector>
#include<iomanip>

#include "wrapper.hpp"
#include "input_parser.hpp"
#include "pedestal.hpp"


using silicio_t = silicio<float>;
using detector_t = std::vector<silicio_t>;

static const int Nstrip=384;
static const int Nmodules=5;

void fill_status(detector_t&, const rapidjson::Document& );

std::string input_file(const rapidjson::Document&);

void sub_cm_asic(silicio_t& detector, int first, int last) {
  
  float gap = ( std::accumulate(detector.data.begin()+first,
                                detector.data.begin()+last,
                                0.0) -
                std::accumulate(detector.spede.begin()+first,
                                detector.spede.begin()+last,
                                0.0)
                );
  int n_value = std::count_if(detector.status.begin()+first,
                              detector.status.begin()+last,
                              [&](bool fail) { return !fail; } );
  float cm=gap/n_value;
  std::for_each(detector.data.begin()+first,
                detector.data.begin()+last,
                [=](float& value) { 
                  if(value > 0)
                    value -= cm; } 
                );
  
}


void scrivi_pede(detector_t& detector, rapidjson::Document& p, bool is_sub=false) {
  
  std::string formatted_output= std::string(p["output_directory"].GetString())+"/"+std::string(p["output"].GetString());
  std::size_t found = formatted_output.find(".");
  formatted_output.replace(found, std::string::npos,".dat");
  std::ofstream of(formatted_output, std::ofstream::out);
  
  if( !is_sub)
    for(auto& d:detector) {
      for(int istrip=0;istrip<d.spede.size();++istrip) 
        of << d.spede[istrip] << "\t" << d.srms[istrip] << "\n";
    }
  else
    for(auto& d:detector) {
      for(int istrip=0;istrip<d.spede.size();++istrip) 
        of << d.spede[istrip]  << "\t" << d.srms[istrip] << "\t" << d.data[istrip] << "\n";
    }

}

void get_data(detector_t& detector, int offset) {
  get_raw_data1_(&detector[0].data[0], &offset);
  get_raw_data2_(&detector[1].data[0], &offset);
  get_raw_data3_(&detector[2].data[0], &offset);
  get_raw_data5_(&detector[3].data[0], &offset);
  get_raw_data6_(&detector[4].data[0], &offset);
}

void fill_histo(int id, detector_t& detector) {
  int uno=1.0f;
 
  for(int isili=0;isili<detector.size();++isili) {
    int hid=id+isili;
    for(int istrip=0;istrip<detector[isili].data.size();++istrip) {
      float value = detector[isili].data[istrip];
      //          hfill_(&hid,&istrip,&value,&uno);
      hfill(hid,istrip,value,uno);
    }
  }

}



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
  
  hbprof(99,"dummy",384,0.,384.);
  for(int isili=0;isili<Nmodules;++isili) {
    id=100+isili;
    float min=0.0f;
    float max=Nstrip;
    float zero=0.0f;
    float maxH=4000.0f;
    char name[4]={'p','e','d','e'};
    char opt='S';
    int n=Nstrip;
    hbprof_(&id,name,&n,&min,&max,&zero,&maxH,&opt,1,1);
    id=200+isili;
    hbprof_(&id,name,&n,&min,&max,&zero,&maxH,&opt,1,1);
  }

  std::vector< silicio<float> > detector;
  for(int nsili=0;nsili<Nmodules;++nsili)
    detector.push_back(silicio<float>(Nstrip));

  std::vector<float> pede(Nstrip);
  std::vector<float> rms(Nstrip);
  
  std::string s=input_file(p);
  std::cout << s << std::endl;
  if(!std::ifstream(s)) {
    throw std::runtime_error("File mancante.\n");
  }
    
  int nmax, hid;
  float strip,value,uno=1.0f,zero=0.0f;;

  {
    apri_tupla_(s.c_str(),nmax,tuple_id);
    if(nmax == -1) 
      return -1;
    
    std::cout << "\n"
              << s << "\t"
              << "max events = " << nmax << std::endl;
    
    for(int iev=1;iev<nmax;++iev) {
      id=1;
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;

      get_data(detector,offset);
      fill_histo(100,detector);
      
    }

    chiudi_tupla_(tuple_id);

    for(int isili=0;isili<detector.size();++isili) {
      id=100+isili;
      hunpak(id, &detector[isili].spede[0]);
      hunpke(id, &detector[isili].srms[0]);
    }

  }


  scrivi_pede(detector,p);
  fill_status(detector,p);


  {
    apri_tupla_(s.c_str(),nmax,tuple_id);
    if(nmax == -1) 
      return -1;

    for(int iev=1;iev<nmax;++iev) {
      id=1;
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;
      
      get_data(detector,offset);


      for( auto& s: detector) {
        std::transform(s.data.begin(), s.data.end(),
                       s.status.begin(),
                       s.data.begin(),
                       [&](float first, bool fail) { 
                         if(fail)
                           return 0.0f;
                         return first;
                       } );
        std::transform(s.spede.begin(), s.spede.end(),
                       s.status.begin(),
                       s.spede.begin(),
                       [&](float first, bool fail) { 
                         if(fail)
                           return 0.0f;
                         return first;
                       } );
      }

      
      std::vector<int> estremi;
      std::vector<int> estremi_basculo;
      std::vector<float> cut;
      for (auto& v : p["estremi_asic"].GetArray())
        estremi.push_back(v.GetInt());
      for (auto& v : p["estremi_asic_basculo"].GetArray())
        estremi_basculo.push_back(v.GetInt());

      for(int isili=0;isili<detector.size(); ++isili) {
        if( isili==2 ) {
          for(int e = 0; e < estremi_basculo.size()-1; ++e) {
            sub_cm_asic(detector[isili],estremi_basculo[e],estremi_basculo[e+1]);
          }
        }
        else {
          for(int e = 0; e < estremi.size()-1; ++e) {
            sub_cm_asic(detector[isili],estremi[e],estremi[e+1]);
          }
        }
        
      }

      fill_histo(200,detector);      
    
    }
    
    chiudi_tupla_(tuple_id);
  }
  
  for(int isili=0;isili<detector.size();++isili) {
    hunpke(200+isili, &detector[isili].data[0],uno);
  }

  scrivi_pede(detector,p,true);
  

  finalize_(&output_id);

  return 0;
}






std::string input_file(const rapidjson::Document& p) {
  std::string s =  std::string(p["input_directory"].GetString()) + 
    "/run"+std::to_string(p["first"].GetInt())+"_pede.hbook";
  std::cout << s << std::endl;
  s.resize(s.size());
  return s;
}






void fill_status(detector_t& detector, const rapidjson::Document& p) {


  // mean
  for(auto& d:detector) {
    double sum = std::accumulate(d.spede.begin(), d.spede.end(),
                                 0.0);
    double sum2 = std::inner_product(d.spede.begin(), d.spede.end(), 
                                     d.spede.begin(),
                                     0.0);
    double mean = sum / d.spede.size();
    double mean2 = sum2 /  d.spede.size();
    double rms = sqrt(mean2-mean*mean);

    for(int istrip=0;istrip<d.spede.size();++istrip) {
      if( d.spede[istrip] < (mean - p["tol_pede"].GetFloat()*rms) ||
          d.spede[istrip] > (mean + p["tol_pede"].GetFloat()*rms) )
        d.status[istrip] = true;
    }
  }
  
  // snr
  for(auto& d:detector) {
    std::vector<float> snr(Nstrip);
    std::transform(d.spede.begin(), d.spede.end(),
                   d.srms.begin(),
                   snr.begin(),
                   std::divides<float>());

    double sum = std::accumulate(snr.begin(), snr.end(),
                                 0.0);
    double sum2 = std::inner_product(snr.begin(), snr.end(), 
                                     snr.begin(),
                                     0.0);
    double mean = sum / snr.size();
    double mean2 = sum2 /  snr.size();
    double rms = sqrt(mean2-mean*mean);

    for(int istrip=0;istrip<snr.size();++istrip) {
      if( snr[istrip] < (mean - p["tol_snr"].GetFloat()*rms) ||
          snr[istrip] > (mean + p["tol_snr"].GetFloat()*rms) )
        d.status[istrip] = true;
    }
  }



}
