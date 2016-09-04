#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <iterator>
#include <numeric>
#include <algorithm>

#include "algo.hpp"

template<class InputIterator,class StatusIterator>
void good_strips(const StatusIterator, const StatusIterator, InputIterator);

template<class InputIterator,class StatusIterator, typename Predicate>
void good_strips(StatusIterator, const StatusIterator, InputIterator, Predicate);

template<class InputIterator,class StatusIterator>
void sub_pede(InputIterator, const InputIterator, InputIterator, StatusIterator);

template<class InputIterator,class StatusIterator, typename Predicate>
void sub_pede(InputIterator, const InputIterator, InputIterator, StatusIterator, Predicate);

// template<class InputIterator, typename Predicate>
// InputIterator sub_cm(InputIterator, const InputIterator, InputIterator, Predicate);

template<class InputIterator, typename T>
T sub_cm(InputIterator, const InputIterator, InputIterator, const T&);

template<class InputIterator,class OutputIterator>
OutputIterator compute_snr(InputIterator,const InputIterator,InputIterator, OutputIterator);


namespace types {

  template<int N>
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
    
    // silicio() { 
    //   value.resize(Nstrip);
    // }
    silicio(array_f& spede_,array_f& srms_,array_b& status_) : spede(spede_),srms(srms_),status(status_){ 
      value.resize(Nstrip);
      snr.resize(Nstrip);
      pre_process();
      process();
    }
    silicio(const silicio& other) : value(other.value), spede(other.spede), srms(other.srms), status(other.status) { snr.resize(Nstrip); };
    
    array_f::iterator begin() { return value.begin(); }
    array_f::iterator end() { return value.end(); }
    array_f::const_iterator begin() const { return value.begin(); }
    array_f::const_iterator end() const { return value.end(); }

    value_type& operator[](const int i) { return value[i]; }

    array_f pre_process(const value_type& cut_ = 0.0f) {
      cut = cut_;
      good_strips(status.begin(),status.end(),begin());
      sub_pede(begin(),end(),spede.begin(),srms.begin(),std::bind2nd(std::greater<value_type>(),0.0f));
      array_f cm;
      cm.push_back(sub_cm(value.begin()    ,value.end()+128,srms.begin()    ,cut));
      cm.push_back(sub_cm(value.begin()+128,value.end()+256,srms.begin()+128,cut));
      cm.push_back(sub_cm(value.begin()+256,value.end()+384,srms.begin()+256,cut));
      return cm;
    }
    
    void process() {
      compute_snr(value.begin(),value.end(),srms.begin(),snr.begin());
      ph_max=algo::max_value(begin(),end());
      pull=algo::max_value(snr.begin(),snr.end());
      eta=algo::eta(begin(),end(),begin()+std::distance(snr.begin(),pull));
    }

  };


}


template<class InputIterator,class StatusIterator>
void good_strips(const StatusIterator begin, const StatusIterator end, InputIterator data) {
  StatusIterator it=begin;
  do {
    it=find(it,end,1);
    if((*it)==1) {
      *(data+std::distance(begin,it))=-1.0;
      //      std::cout << std::distance(begin,it) << "\t" << data[std::distance(begin,it)] << "\t" << (*it) << std::endl;
    }
  }while (++it<end);
}

template<class InputIterator,class StatusIterator, typename Predicate>
void good_strips(StatusIterator begin, const StatusIterator end, InputIterator data, Predicate Pred) {
  StatusIterator it(begin);
  do {
    it=find(it,end,Pred(*it));
    if(Pred(*it)) {
      *(data+std::distance(begin,it))=-1.0;
      //      std::cout << std::distance(begin,it) << "\t" << data[std::distance(begin,it)] << "\t" << Pred(*it) << std::endl;
    }
  }while (it!=end);
  it=end-1;
  if(Pred(*it))
    *(data+std::distance(begin,it))=-1.0;
}


template<class InputIterator,class StatusIterator>
void sub_pede(InputIterator begin, const InputIterator end, InputIterator pede, StatusIterator st) {
  InputIterator it=begin;
  do {
    if( (*st)==0 )
      (*it) = std::max( (*it) - (*pede),0.0f);
    ++pede;
    ++st;
  }while (++it!=end);
}

template<class InputIterator,class StatusIterator, typename Predicate>
void sub_pede(InputIterator begin, const InputIterator end, InputIterator pede, StatusIterator st, Predicate Pred) {
  InputIterator it=begin;
  do {
    if(Pred(*it))
      (*it) = std::max( (*it) - (*pede),0.0f);
    ++pede;
    ++st;
  }while (++it!=end);
}



//////////////////////
// Assume adc gia' sottratta del pede e "spente" le strip "morte" 
template<class InputIterator, typename T>
T sub_cm(InputIterator begin, const InputIterator end, InputIterator rms, const T& cut=5.) {
  T gap,cm(0);
  int count = 0;

  InputIterator i(begin);
  InputIterator r(rms);
  do{
    if ( ((*i) < cut*(*r)) && ((*i)>0) ) {
      cm+=*i;
      ++count;
    }
  }while(++r,++i!=end);

  if(count > 0) {
    cm/=static_cast<double>(count);
    i=begin;
    r=rms;
    do{
      if ( ((*i) < cut*(*r)) && ((*i)>0) )
        (*i)-=cm;
      if( (*i) < 0 ) (*i)=0;
    }while(++r,++i!=end);
  }
  return cm;
}


template<class InputIterator,class OutputIterator>
OutputIterator compute_snr(InputIterator begin,const InputIterator end,InputIterator rms, OutputIterator snr) {
  do {
    if(*rms>0)
      (*snr)=(*begin)/(*rms);
  }while(++rms,++snr,++begin!=end);
  return snr;
}
