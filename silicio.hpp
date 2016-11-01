#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <algorithm>

#include "algo.hpp"

template<class Input>
void subtract_commonmode(Input&, const int, const int, float);

template<class Input, typename T>
T compute_cm(Input&, const int, const int, const T&);

template<class Input>
void subtract_pedestal(Input&);
    
template<class InputIterator,class StatusIterator>
void good_strips(const StatusIterator, const StatusIterator, InputIterator);

template<class InputIterator,class StatusIterator, typename Predicate>
void good_strips(StatusIterator, const StatusIterator, InputIterator, Predicate);


template<class InputIterator,class OutputIterator>
OutputIterator compute_snr(InputIterator,const InputIterator,InputIterator, OutputIterator);


namespace types {

  template<int N,int Nasic=3>
  struct silicio {
    static const int Nstrip=N;
    typedef float value_type;

    typedef typename std::vector<value_type> array_f;
    typedef typename std::vector<bool> array_b;

    array_f value;
    array_f snr;
    array_f& spede;
    array_f& srms;
    array_b& status;    
    
    array_f::iterator ph_max;
    array_f::iterator pull;
    value_type eta;
    value_type cut;
    
    silicio(array_f& spede_,array_f& srms_,array_b& status_) : spede(spede_),srms(srms_),status(status_){ 
      value.resize(Nstrip);
      snr.resize(Nstrip);
      //      good_strips(status.begin(),status.end(),begin());
      //      subtract_pedestal(*this);
    }
    silicio(const silicio& other) : value(other.value), spede(other.spede), srms(other.srms), status(other.status) { snr.resize(Nstrip); };
    
    array_f::iterator begin() { return value.begin(); }
    array_f::iterator end() { return value.end(); }
    array_f::const_iterator begin() const { return value.begin(); }
    array_f::const_iterator end() const { return value.end(); }

    value_type& operator[](const int i) { return value[i]; }

  };

}



template <typename Silicio>
void pre_process(Silicio& sili, const float& soglia, const float& cut) {
  std::array<float,Silicio::Nstrip> gap;

  // zeros dead strips
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.spede.begin(),
                 sili.value.begin(),
                 [&](float arg1, bool arg2) { 
                   return (arg2 > 0? arg1 : 0.0); });


  // subtracts pede
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.spede.begin(),
                 gap.begin(),
                 std::minus<float>());
        
  {
    const int Nasic=3;
    std::array<float,Nasic> cm,n;
    for(int iasic=0;iasic<Nasic;++iasic) {
            
      for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
        if( gap[istrip] <soglia*sili.srms[istrip] && !sili.status[istrip] ) {
          cm[iasic] += gap[istrip];
          n[iasic]++;
        }
      cm[iasic]/=n[iasic];
      for(int istrip = 128*iasic; istrip < 128*(iasic+1);++istrip)
        if(!sili.status[istrip])
          sili.value[istrip] = gap[istrip] - cm[iasic];         
    }
  }
  // snr
  std::transform(sili.value.begin(),sili.value.end(),
                 sili.srms.begin(),
                 sili.snr.begin(),
                 [&](float n, float d) { 
                   return (d > 0? n/d : 0.0); });
  
} // pre_process



// template <typename Silicio>
// void pre_process_basculo(Silicio& sili, const float& soglia, const float& cut) {
//   std::array<float,Silicio::Nstrip> gap;
