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

// template<class Input, typename T>
// T compute_cm(Input&, const int, const int, const T&);

template<class Input, typename T, typename Predicate>
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
      good_strips(status.begin(),status.end(),begin());
      //      subtract_pedestal(*this);
    }
    silicio(const silicio& other) : value(other.value), spede(other.spede), srms(other.srms), status(other.status) { snr.resize(Nstrip); };
    
    array_f::iterator begin() { return value.begin(); }
    array_f::iterator end() { return value.end(); }
    array_f::const_iterator begin() const { return value.begin(); }
    array_f::const_iterator end() const { return value.end(); }

    value_type& operator[](const int i) { return value[i]; }

    array_f pre_process(const value_type& cut_ = 0.0f) {
      cut = cut_;
      array_f cm;
      for(int i=0;i<Nasic;++i) {
        cm.push_back(compute_cm(*this,128*i,128*(i+1),cut));
        subtract_commonmode(*this,128*i,128*(i+1),cm[i]);
      }
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




//////////////////////
// Sottrae il pede dai dati raw
template<class Input>
void subtract_pedestal(Input& sili) {
  float t;
  for(int i=0;i<sili.value.size();++i) {
    if(sili.spede[i] < 1)
      sili.value[i] = -1;
    else {
      sili.value[i] -= sili.spede[i];
      if (sili.value[i] < 0)
        sili.value[i] = 0;
    }
  }
}


template <typename T>
struct CMselector_pede {
  CMselector_pede(const T& selection_cut) : c(selection_cut) { };
  bool operator()(const T& value) const {
    return (value > 0);
  }
  
private :
  const T& c;
};

template <typename T>
struct CMselector_event {
  CMselector_event(const T& selection_cut) : c(selection_cut) { };
  bool operator()(const T& value) const {
    return (value < c && value>0);
    //    return false;
  }
  
private :
  const T& c;
};

//////////////////////
// Assume adc gia' sottratta del pede e "spente" le strip "morte"; calcola il cm e lo ritorna
template<class Input, typename T,template <typename Type> class Predicate>
T compute_cm(Input& sili, const int first, const int last, const T& cut) {

  Predicate<T> Pred(cut);
  float tmp_cm = 0;
  int n=0;
  for(int i = first;i<last;++i) {
    //    if(sili.value[i] < 1.*cut && sili.value[i]>0) {
    if( Pred(sili.value[i]) ) {
      tmp_cm += sili.value[i];
      n++;
    } 
  }
  return tmp_cm/n;
}
//////////////////////
// Assume adc gia' sottratta del pede e "spente" le strip "morte". Sottrae cm
template<class Input>
void subtract_commonmode(Input& sili, const int first, const int last, float cm) {
  for(int i=0;i<sili.value.size();++i) {
    if( sili.value[i] > 0)
      sili.value[i] -= cm;
    if (sili.value[i] < 0)
      sili.value[i] = 0;
  }
}




template<class InputIterator,class OutputIterator>
OutputIterator compute_snr(InputIterator begin,const InputIterator end,InputIterator rms, OutputIterator snr) {
  do {
    if(*rms>0)
      (*snr)=(*begin)/(*rms);
  }while(++rms,++snr,++begin!=end);
  return snr;
}
