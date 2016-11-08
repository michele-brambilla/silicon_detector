#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

struct cluster {
  float x_cm;
  float x_pos;
};


struct sili_stats {

  int max_strip;
  float ph;
  float pull;
  float pullL;
  float pullR;
  float eta;
  int n_cluster;
  std::vector<cluster> c;

};




struct over_threshold {
  over_threshold(const over_threshold& other) : rms(other.rms), cut(other.cut) { };
  over_threshold(std::vector<float>& r, float c) : rms(r), cut(c) { }
  float operator()(std::vector<float>& x, const int min, const int max) {
    float result = 0.0f;
    for(int i=min;i<max;++i) {
      if( x[i] > cut*rms[i] )
        result += x[i];
    }
    return result;
  }

  template<class BinaryOperator>
  float operator()(std::vector<float>& x, const int min, const int max, 
                   BinaryOperator op) {
    float result = 0.0f;
    for(int i=min;i<max;++i) {
      if( x[i] > cut*rms[i] )
        result = op(result,x[i]);
    }
    return result;
  }

  void pretty_print(std::vector<float>& x, const int min, const int max) {
    for(int i=min;i<max;++i) {
      if( x[i] > cut*rms[i] )
        std::cout << "\t" << x[i] << "(" << cut*rms[i] << ")";
      else
        std::cout << "\t" << 0;
    }
    std::cout << "\n";
  }
  
  
  std::vector<float>& rms;
  float cut;
};


template<typename T>
struct silicio {
  typedef T value_t;
  typedef typename std::vector<value_t> value_vector;
  typedef typename std::vector<value_t> bool_vector;

  value_vector	 data;
  value_vector	 spede;
  value_vector	 srms;
  bool_vector status ;
  sili_stats info;
  float cut;

  silicio(const int nstrip) : cut(0.0f) {
    data.resize(nstrip);
    spede.resize(nstrip);
    srms.resize(nstrip);
    status.resize(nstrip);
  }
  silicio(const value_vector& p, const value_vector& r, const bool_vector& s, const float& c) : spede(p), srms(r), status(s), cut(c) {
    data.resize(spede.size());
  }
  value_t& operator[](const int n) { return data[n]; }

  void basic_info(float cut_low) {
    int distance = std::distance(std::max_element(data.begin(),
                                                  data.end() ),
                                 data.end() );
    if( distance == data.size() || data[distance] <= cut*srms[distance] ) {
      info.max_strip = -1;
      info.ph = -1;
      info.pull = -1;
      info.pullL = -1;
      info.pullR = -1;
      info.eta = -10;
      return;
    }
    info.max_strip = distance;
    info.ph = data[info.max_strip];
    info.pull = info.ph/srms[info.max_strip];
    
    float other;
    if (distance == 0) {
      other = data[info.max_strip+1]; // (R)
      info.pullL =-1;
      info.pullR = other;
    }
    else {
      if( distance == data.size()-1 ) {
        other = data[info.max_strip-1];
        info.pullL = other;
        info.pullR = -1;
      }
      else {
        other = std::max(data[info.max_strip-1],data[info.max_strip+1]);
        info.pullL = data[info.max_strip-1];
        info.pullR = data[info.max_strip+1];
      }
    }

    if( info.pullL < cut_low) info.pullL = -1;
    if( info.pullR < cut_low) info.pullR = -1;
    info.eta = (info.ph-other)/(info.ph+other);
    
  }


};




struct common_mode {
  common_mode(std::vector<float>& p, std::vector<float>& r, 
              float c=5.0f) : pede(p), rms(r), cut(c) { };
  
  float get_cm_value(std::vector<float>& data, const int first, const int last) {
    float gap,cm=0;
    int n_count=0;
    for(int istrip=first;istrip < last;++istrip) {
      if(pede[istrip] > 0.0f) {
        gap = data[istrip] - pede[istrip];
        if(gap < cut*rms[istrip]) {
          n_count++;
          cm += gap;
        }
      }
    }
    if(n_count != 0)
      return cm/n_count;
    return 0;
  }

  std::vector<float>::iterator subtract_cm_value(std::vector<float>& data, 
                                                 const int first, const int last,
                                                 const float& cm) {
    for(int istrip=first;istrip < last;++istrip) {
      if(pede[istrip] > 0.0f) {
        data[istrip] = std::max(data[istrip]-pede[istrip]-cm,0.0f);
      }
      else {
        data[istrip] = 0.0f;
      }
    }
    return data.begin()+first;
  }

  std::vector<float>& pede;
  std::vector<float>& rms;
  float cut;
};
