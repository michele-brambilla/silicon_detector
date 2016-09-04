#ifndef _ALGO_H
#define _ALGO_H

#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <algorithm>


namespace algo {

  //////////////////////////
  // utile per ph e pull
  template<class InputIterator>
  InputIterator max_value(InputIterator begin,const InputIterator end) {
    return std::max_element(begin,end);
  }


  template<class InputIterator>
  typename InputIterator::value_type eta_impl(const InputIterator first, 
                                              const InputIterator second, 
                                              const typename InputIterator::value_type init) {
    return std::max(init,( (*first)-(*second) )/( (*first)+(*second) ));
  }
  
  template<class InputIterator, int Handness=0>
  typename InputIterator::value_type eta(InputIterator begin,
                                         const InputIterator end,
                                         const InputIterator pull) {
    typename InputIterator::value_type l,r;
    if(pull>begin)
      l=eta_impl(pull-1,pull,-1);
    if(pull<end)
      r=eta_impl(pull,pull+1,-1);
    // std::cout << std::endl
    //           << "l = " << (*(pull-1)) << "\t"
    //           << "c = " << (*pull) << "\t"
    //           << "r = " << (*(pull+1)) << "\t"
    //           << "[ " <<  std::max(l,r) << " ]"
    //           << std::endl;
    if( (-l)>r)
      return l;
    return r;
  }
  
}

#endif //ALGO_H
