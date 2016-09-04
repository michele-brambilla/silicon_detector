#ifndef _CLUSTER_H
#define _CLUSTER_H
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <algorithm>

#include <histo.hpp>


namespace algo {

  template<typename cluster>
  struct cluster_info {
    typedef typename cluster::array_t array_t;
    typedef typename array_t::iterator iterator_t;

    iterator_t first;
    iterator_t last;
  };

  template<typename silicio_t, typename Predicate>
  struct cluster {

    typedef typename silicio_t::value_type data_t;
    typedef typename std::vector<data_t> array_t;
    typedef cluster<silicio_t,Predicate> self_t;

    //    cluster() { }
    cluster(std::vector<data_t>& d_, 
            std::vector<data_t>& r_, std::vector<bool>& s_) : d(d_), r(r_), s(s_) { }
    cluster(silicio_t& source) : d(source.value), r(source.srms), s(source.status) { }
    cluster(self_t& other) : d(other.d), r(other.r), s(other.s) { }

    void operator()(const data_t& cut) {
      typename array_t::iterator it=d.begin();
      typename array_t::iterator rms=r.begin();
      std::vector<bool>::iterator stat=s.begin();
      Predicate Pred;

      do {

        it=Pred(it,d.end(),rms,stat,cut);
        std::cout << (*it) << std::endl;

      } while(++it!=d.end());



    }

    std::vector<data_t>& d;
    std::vector<data_t>& r;
    std::vector<bool>& s;
    std::vector<cluster_info<self_t> > info;

  };


  template <typename InputIterator, typename StatusIterator>
  struct AllOnCluster {
    AllOnCluster() { };
    InputIterator operator()(InputIterator first, 
                             const InputIterator last,
                             InputIterator rms, 
                             StatusIterator s,
                             typename InputIterator::value_type cut) {
      do {
        ++rms;
        ++s;
      } while (++first!=last);
      
      return first;
    }
  };



  struct cluster_data {
    int seed;
    int begin;
    int end;
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
    // std::cout << cluster.size() << std::endl;
    // if( cluster.size() )
    //   std::cout << cluster[0].begin << "\t" << cluster[0].end << std::endl;
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
  void process_cluster(cluster_data& cluster, const S& silicio) {
    cluster.seed = std::distance(silicio.begin(),std::max_element(silicio.begin()+cluster.begin,silicio.begin()+cluster.end));
    int nr_strip    = cluster.end - cluster.begin;
    float amplitude = std::accumulate(silicio.begin()+cluster.begin,silicio.begin()+cluster.end,0.);
    float noise2 = std::inner_product(silicio.srms.begin()+cluster.begin,
                                      silicio.srms.begin()+cluster.end,
                                      silicio.srms.begin()+cluster.begin,
                                      0.)/nr_strip;

    hf1(150,amplitude);
    hf1(160,amplitude/std::sqrt(noise2));
    hf1(170,nr_strip);
    
  }



}






#endif //CLUSTER_H
