#ifndef _TYPES_H
#define _TYPES_H

#include <iostream>
#include <vector>
#include <array>

namespace types {

  template<class data_t,class iterator_t, class biterator_t>
  data_t raw_common_mode(iterator_t first, iterator_t last, 
                         iterator_t pede, iterator_t rms, 
                         biterator_t status, const data_t& cut) {
    iterator_t p=pede;
    iterator_t r=rms;
    biterator_t s=status;
    data_t gap,cm=0.;
    int chan_count=0;

    for(iterator_t i=first;i!=last;++i,++p,++r,++s) {
      gap = *i - *p;
      if( gap > cut*(*r) && (*s)==0) {
        cm += gap;
        ++chan_count;
      }
    }

    if(chan_count > 0)
      return cm/static_cast<float>(chan_count);
    else
      return 0.;
  }



  template <class T, int N>
  struct data_silicio {
    typedef T data_t;
    typedef std::vector<data_t> array_t;
    typedef std::vector< std::array<data_t,2> > cut_t;
    typedef data_silicio<data_t,N> self_t;
    typedef typename array_t::iterator iterator;
    typedef typename array_t::const_iterator const_iterator;
    typedef std::vector<bool> bool_t;
    typedef typename bool_t::iterator biterator;

    array_t data;
    array_t pede;
    array_t rms;
    cut_t cut;
    bool_t status;

    iterator begin() { return data.begin(); }
    const_iterator begin() const { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator end() const { return data.end(); }

    data_silicio(const int Nstrip=N) { data.resize(Nstrip); pede.resize(Nstrip); rms.resize(Nstrip); status.resize(Nstrip); };
    data_silicio(array_t& data_,const int Nstrip=N) : data(data_) { pede.resize(Nstrip); rms.resize(Nstrip); status.resize(Nstrip); };
    data_silicio(array_t& data_, array_t& pede_,array_t& rms_,
                 cut_t& cut_,bool_t& status_) : data(data_),pede(pede_),rms(rms_),cut(cut_),status(status_) { };
    data_silicio(self_t& other) : data(other.data),pede(other.pede),rms(other.rms),cut(other.cut),status(other.status){ };

    data_t* c_data(void) { return &data[0]; }
    
    const data_t primary_cut(const int izone=0) { return cut[izone][0]; }
    const data_t secondary_cut(const int izone=0) { return cut[izone][1]; }

    self_t& sub_pede(const int nasic, const data_t cut=5.) {
      data_t cm;
      const int nchan=static_cast<int>(data.size()/nasic);
      int off1,off2;
      
      for(int iasic=0;iasic<nasic;++iasic) {
        off1=nchan*iasic;
        off2=std::min(nchan*(iasic+1),static_cast<int>(data.size()));
        cm=raw_common_mode(data.begin()+off1,data.begin()+off2,
                           pede.begin()+off1,rms.begin()+off1,
                           status.begin()+off1,
                           cut);
        sub_pede_impl(data.begin()+off1,data.begin()+off2,
                      pede.begin()+off1,status.begin()+off1,
                      cm);
      }
      return *this;//sub_pede_impl(nasic);
    }

    self_t& disable_no_good() {
      // biterator i = status.begin();
      // iterator x;
      // do {
      //   *x=*x;
      //   //        if(*i==1) *x = -1.;
      // } while ((++i) != status.end(),++x);
      // return *this;

    }

  private:

    void sub_pede_impl(iterator first, iterator last, iterator p, biterator s, const data_t& cm) {
      
      for(iterator i=first;i!=last;++i,++p,++s) {
        if( (*s) == 0 ) {
          *i -= (*p) + cm;
          if( *i < 0. ) *i=-1.;
        }
        else
          *i = -1.;
      }
    }

  };




  

  
  template<typename T,int N>
  struct basic_data {

    typedef T data_t;
    typedef typename std::vector<T> array_t;
    typedef basic_data<T,N> self_t;
    
    typedef typename array_t::iterator iterator;
    typedef typename array_t::const_iterator const_iterator;
    
    array_t d;
    iterator begin() { return d.begin(); }
    const_iterator begin() const { return d.begin(); }
    iterator end() { return d.end(); }
    const_iterator end() const { return d.end(); }

    basic_data(const int Nstrip=N) { d.resize(N); };
    basic_data(const self_t& other) : d(other) { };
    
    T& operator[](const int i) { return d[i]; }

  };
  

  template<typename T>
  struct info_t {

    typedef typename T::data_t data_t;
    typedef typename T::iterator iterator;
    typedef typename T::const_iterator const_iterator;
    
    data_t max_value;
    int imax;
  };


}

#endif //TYPES_H
