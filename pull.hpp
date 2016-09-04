#ifndef _PULL_H
#define _PULL_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "types.hpp"

template <typename T, typename I>
typename T::const_iterator pulse_height(const T& data, I& info) {

  typename T::const_iterator imax= std::max_element(data.begin(),data.end());
  int distance=std::distance(data.begin(),imax);

  if( (*imax) > data.rms[distance]*data.primary_cut() ) {
    info.imax=distance;
    info.max_value=*imax;
  }
  else {
    info.imax=-1;
    info.max_value=-1;
    imax=data.end();
  }

  return imax;
}


template <typename T, typename I>
typename T::const_iterator pull(const T& data,const T& rms, I& info) {
  T ratio;
  std::transform(data.begin(),data.end(),rms.begin(),ratio.begin(),std::divides<typename T::data_t>());

  typename T::const_iterator imax= std::max_element(ratio.begin(),ratio.end());
  int distance=std::distance(ratio.begin(),imax);

  if( (*imax) > data.primary_cut() ) {
    info.imax=distance;
    info.max_value=*imax;
  }
  else {
    info.imax=-1;
    info.max_value=-1;
    imax=data.end();
  }

  return imax;
}



#endif //PULL_H
