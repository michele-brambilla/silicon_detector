#ifndef _CERNRUN_H
#define _CERNRUN_H

#include "uparam.hpp"
#include "types.hpp"


////////////////////////////////
// hbook, fortran2c
#include "cfortran.h"
#include "cfortran/hbook.h"


#include <string>
#include <type_traits>




extern "C" {
  void init_(const char*, int*); 
  void apri_tupla_(const char*, int&, int&);
  int chiudi_tupla_(int&);
  void finalize_(int*);
  void get_raw_data_(float*,float*,float*,const int*);
  void get_raw_data1_(float*,const int*);
  void get_raw_data2_(float*,const int*);
  void get_raw_data3_(float*,const int*);
  void get_raw_data4_(float*,const int*);
  void get_raw_data5_(float*,const int*);
  void get_raw_data6_(float*,const int*);
  void plot_(int*);

  //  void hbprof_(int*,char*,int*,float*,float*,float*,float*,char*);
  void hgnt_(int*, int*, int*);
  //  void hfill_(int*, int*, float*,float*);
  //  void hf1_(int*, float*, float*);
  //  void hunpak_(int*, float*, float*);
  //  void hunpke_(int*, float*, float*);

  void prepara_histo_(int*);
}

extern "C" {

  void led_event_(int *);
  bool is_led_event();
  
  void hbook1(int, std::string, int, float, float, float=0.);
  void hbook2(int, std::string,int, float, float,int, float, float,float=0.);
  
  void hbprof(int, std::string,int, float, float,float=4000.);
  
  //  void hf1(int, float*, float=1.0f);
  //  void hf2(int, float, float, float=1.0f);
  
  void hfill(int, int, float,float=1.f);
  
  void hunpak(int, float*, int=1);
  void hunpke(int, float*, int=1);
  void hpak(int, float*);

  // void hplot(int id) {
  //   plot_(&id);
  // }
  
}

void prepara_histo(const int);



#endif //CERNRUN_H
