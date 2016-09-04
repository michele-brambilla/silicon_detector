#ifndef _CLUSTER_H
#define _CLUSTER_H
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <algorithm>

// fake histograms
//#include <histo.hpp>

#include "cernrun.hpp"


namespace algo {


  struct cluster_data {
    int seed;
    int begin;
    int end;

    float amplitude;
    float snr;
    
    float eta;
    float pullL;
    float pullR;
    
    float x_cm;
    float x_pos;

    void fill_histo(const int i, float pitch) {
      hf1(120+i,pullL);
      hf1(130+i,pullR);
      hf1(140+i,eta);
      
      hf1(150+i,amplitude);
      hf1(160+i,snr);
      hf1(170+i,end-begin);
    }

    void fill_histo_hit(const int i, float pitch) {
      
      hf1(190+i,x_cm);
      hf1(200+i,x_cm-std::floor(x_cm/pitch)*pitch);
      hf1(210+i,x_pos);
      hf1(220+i,x_pos-std::floor(x_cm/pitch)*pitch);


    }
  };

  template <typename S>
  std::vector<cluster_data>  clusterize(S& s,
                   float cut) {

    std::vector<cluster_data> cluster;
    cluster_data c;
    
    bool in_cluster = false;
    int last_good = -1;
    typename S::array_f::iterator rms = s.srms.begin();
    typename S::array_b::iterator stat = s.status.begin();

    for(typename S::array_f::iterator it = s.begin(); it != s.end(); ++it, ++rms, ++stat) {

      if( *stat )
        continue;
      
      if( !in_cluster && (*it) > (*rms)*cut ) {
        in_cluster = true;
        c.begin=std::distance(s.begin(),it);
      }
      if( in_cluster && (*it) < (*rms)*cut ) {
        in_cluster = false;
        c.end=std::distance(s.begin(),it);
        cluster.push_back(c);
      }

    }
    return std::move(cluster);
  }


  
  template<typename S>
  float eta(const int& seed, const S& silicio) {    
    int L = seed-1,R = seed+1;

    if( L < 0 )
      return (silicio.value[seed] - silicio.value[R])/(silicio.value[seed] + silicio.value[R]);
    else
      if( R >= S::Nstrip ||  silicio.value[L] > silicio.value[R] )
        return (silicio.value[L] - silicio.value[seed])/(silicio.value[L] + silicio.value[seed]);
      else
        return (silicio.value[seed] - silicio.value[R])/(silicio.value[seed] + silicio.value[R]);

    throw std::runtime_error("eta value not acceptable");
    return -2;
  }

  
  template<typename S>
  void process_cluster(cluster_data& cluster,
                       const S& silicio,
                       typename S::value_type cut_low,
                       typename S::value_type pitch) {
    cluster.seed = std::distance(silicio.begin(),std::max_element(silicio.begin()+cluster.begin,silicio.begin()+cluster.end));

    //////////////
    // basics
    int nr_strip    = cluster.end - cluster.begin;
    cluster.amplitude = std::accumulate(silicio.begin()+cluster.begin,
                                        silicio.begin()+cluster.end,
                                        0.);
    
    typename S::value_type noise2 = std::inner_product(silicio.srms.begin()+cluster.begin,
                                      silicio.srms.begin()+cluster.end,
                                      silicio.srms.begin()+cluster.begin,
                                      0.)/nr_strip;
    cluster.snr = cluster.amplitude/std::sqrt(noise2);
    
    //////////////
    // charge diffusion
    cluster.eta = eta(cluster.seed, silicio);
    typename S::value_type p;
    if(cluster.seed > 0) {
      p = silicio.value[cluster.seed-1]/silicio.srms[cluster.seed-1];
      if(p>cut_low)
        cluster.pullL = p;
    }
    if(cluster.seed < silicio.value.size()-1) {
      p = silicio.value[cluster.seed+1]/silicio.srms[cluster.seed+1];
      if(p>cut_low)
        cluster.pullR = p;
    }


    /////////////
    // hit point
    std::vector<float> position(nr_strip);
    std::iota(position.begin(),position.end(),0);
    float cm = std::inner_product(silicio.begin()+cluster.begin, silicio.begin()+cluster.end,
                                  position.begin(),
                                  0.)/cluster.amplitude;
    cluster.x_cm = (cm+cluster.begin)*pitch;
  }


}






#endif //CLUSTER_H
