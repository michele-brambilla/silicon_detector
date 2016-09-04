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
#include "cfortran/paw.h"


typedef float float_t;

extern "C" {
  void init_(const char*); 
  void apri_tupla_(const char*, int&);
  int chiudi_tupla_();
  void finalize_(void);
  void get_raw_data_(float_t*,float_t*,float_t*,const int*);
}

extern "C" {

  void led_event_(int *);

  bool is_led_event() {
    int l;
    led_event_(&l);
    if(l<0) return true;
    return false;
  }

  // // call hbook1(isilicio*100000+9,'Pull-cluster',1000,0.5,1500.5           ,0.)
  void hbook1(int id, std::string name,
              int nchan, float_t min, float_t max, 
              float_t other=0.) {
    unsigned int dummy(0);
    hbook1_(&id,&name[0],&nchan,(float*)&min,(float*)&max,(float*)&other,dummy);
  }

  // //  call hbook2(310000,'PH-2d',      20,         0.5,  20.5,1500,0.5,1500.5,0.)
  // void hbook2_(const int*, const char*, 
  //              const int*, const float_t*, const float_t*, 
  //              const int*, const float_t*, const float_t*, 
  //              const float_t*);
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
  
  // //  call hf1(100000*isilicio,subraw(istrip,isilicio),1.0)
  void hf1(int id, float_t x, float_t y=1.0f) {
    hf1_(&id,&x,&y);
  }
  void hf2(int id, float_t x, float_t y, float_t w=1.0f) {
    hf2_(&id,&x,&y,&w);
  }

}


enum { tele1x,basculo,tele2x};

#endif //CERNRUN_H
