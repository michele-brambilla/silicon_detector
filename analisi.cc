#include <map>
#include <functional>

#include "analisi.hpp"

std::vector<common_mode> common;


std::vector<float> calcola_subraw(silicio_t&, std::vector<float>, const float& cut = 5.);
float sub_cm_asic(silicio_t&, const int, const int, const float&);

void analisi_dati(detector_t& detector, const rapidjson::Document& p) {
  float value, uno=1.0f, zero=0.0f;
  int hid;
  int nstrip  = detector[0].data.size();

  std::vector<int> estremi;
  std::vector<int> estremi_tele;
  std::vector<int> estremi_basculo;
  std::vector<float> cut_low;
  for (auto& v : p["asic_tele"].GetArray())
    estremi_tele.push_back(v.GetInt());
  for (auto& v : p["asic_basculo"].GetArray())
    estremi_basculo.push_back(v.GetInt());
  for (auto& v : p["cut_low"].GetArray())
    cut_low.push_back(v.GetFloat());


  for(int isili=0; isili<detector.size(); isili++ ) {

    if( isili==2 ) {
      estremi = estremi_basculo;
    }
    else {
      estremi = estremi_tele;
    }
    float cm_value;
    for(int iasic=0;iasic<estremi_tele.size()-1;++iasic) {
      cm_value = common[isili].get_cm_value(detector[isili].data,estremi_tele[iasic],estremi_tele[iasic+1]);
      common[isili].subtract_cm_value(detector[isili].data,estremi_tele[iasic],estremi_tele[iasic+1],cm_value);
    }

    detector[isili].basic_info(cut_low[isili]);

    if( detector[isili].info.max_strip < 0 ) continue;

    hf1(100+isili,detector[isili].info.ph       ,1.0);
    hf1(110+isili,detector[isili].info.pull     ,1.0);
    hf1(120+isili,detector[isili].info.pullL    ,1.0);
    hf1(130+isili,detector[isili].info.pullR    ,1.0);
    hf1(140+isili,detector[isili].info.eta      ,1.0);

    
    static const int max_cluster_half_size=5;
    int low=std::max(0,
                     detector[isili].info.max_strip-max_cluster_half_size);
    int high=std::min(384,
                      detector[isili].info.max_strip+max_cluster_half_size+1);

    over_threshold accumulate(detector[isili].srms,detector[isili].cut);

    // std::cout << "(" << isili << ")\t"
    //           << low << "\t"
    //           << detector[isili].info.max_strip << "\t"
    //           << high << "\t -> "
    //           << std::accumulate(detector[isili].data.begin()+low,
    //                              detector[isili].data.begin()+high, 0.0f) << "\t"
    //           << accumulate(detector[isili].data,low,high) << "\t"      
    //           << "\n";
    // accumulate.pretty_print(detector[isili].data,low,high);

    // std::cout << "\t";
    // std::copy (detector[isili].srms.begin()+low, 
    //            detector[isili].srms.begin()+high, 
    //            std::ostream_iterator<float>(std::cout, "\t"));
    // std::cout << std::endl;
    // std::cin >> low;

  }

  

}




