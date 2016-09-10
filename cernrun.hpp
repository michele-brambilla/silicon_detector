#ifndef _CERNRUN_H
#define _CERNRUN_H

#include "uparam.hpp"
#include "types.hpp"
#include "util.hpp"
#include "pedestal.cc"
#include "pull.hpp"

////////////////////////////////
// hbook, fortran2c
#include "cfortran/packlib.h"
//#include "cfortran/paw.h"


typedef float float_t;

extern "C" {
  void init_(const char*); 
  void apri_tupla_(const char*, int&);
  int chiudi_tupla_();
  void finalize_(void);
  void get_raw_data_(float_t*,float_t*,float_t*,const int*);
  void get_raw_data1_(float_t*,const int*);
  void get_raw_data2_(float_t*,const int*);
  void get_raw_data3_(float_t*,const int*);
  void get_raw_data4_(float_t*,const int*);
  void get_raw_data5_(float_t*,const int*);
  void get_raw_data6_(float_t*,const int*);

  // void hfill_c(int id,float x,float y, float w) {
  //   hfill_c_(&id, &x, &y, &w);
  // }
  void hf1_c(int*, float*);
}

extern "C" {

  void led_event_(int *);

  bool is_led_event() {
    int l;
    led_event_(&l);
    if(l<0) return true;
    return false;
  }

  void hbook1(int id, std::string name,
              int nchan, float_t min, float_t max, 
              float_t other=0.) {
    unsigned int dummy(0);
    hbook1_(&id,&name[0],&nchan,(float*)&min,(float*)&max,(float*)&other,dummy);
  }

  void hbook2(int id, std::string name,
              int nchan_x, float_t x_min, float_t x_max, 
              int nchan_y, float_t y_min, float_t y_max, 
              float_t other=0.) {
    unsigned int dummy(0);
    hbook2_(&id,&name[0],
            &nchan_x,(float*)&x_min,(float*)&x_max,
            &nchan_y,(float*)&y_min,(float*)&y_max,
            (float*)&other,dummy);
  }
  
  void hbprof(int id, std::string name,
              int nchan_x, float_t x_min, float_t x_max,
              float max_value=4000.) {
    float_t dummy(0);
    float_t min_value = 0;
    char opt='S';
    hbprof_(&id, &name[0], &nchan_x, (float*)&x_min,(float*)&x_max, (float*)&min_value,(float*)&max_value, &opt,0,0);

  }

  
  void hf1(int id, float_t x, float_t weight=1.0f) {
    hf1_(&id,&x,&weight);
  }
  void hf2(int id, float_t x, float_t y, float_t w=1.0f) {
    hf2_(&id,&x,&y,&w);
  }
  
  void hfill(int id, int istrip, float value,float weight = 1.0f) {
    float fstrip=istrip;
    hfill_(&id,&fstrip,&value,&weight);
  }
  
  void hunpak(int id, float_t* x, int other=0) {
    char c=' ';
    hunpak_ (&id,x,&c,&other,1);
  }
  void hunpke(int id, float_t* x, int other=0) {
    char c=' ';
    hunpke_ (&id,x,&c,&other,1);
  }
  void hpak(int id, float_t* x) {
    char c=' ';
    hpak_ (&id,x);
  }

  
}


enum { tele1x,basculo,tele2x};

#endif //CERNRUN_H
