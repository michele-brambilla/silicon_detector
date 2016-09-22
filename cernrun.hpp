#ifndef _CERNRUN_H
#define _CERNRUN_H

#include "uparam.hpp"
#include "types.hpp"
#include "util.hpp"
#include "pedestal.cc"
#include "pull.hpp"

////////////////////////////////
// hbook, fortran2c
//#include "cfortran/packlib.h"

#include <cfortran/paw.h>
#include <cfortran/hbook.h>

#include <type_traits>


const int Nstrip=384;
const int Nsili=4;
const int n_eta_bin = 100;



extern "C" {
  void init_(const char*); 
  void apri_tupla_(const char*, int&);
  int chiudi_tupla_();
  void finalize_(void);
  void get_raw_data_(float*,float*,float*,const int*);
  void get_raw_data1_(float*,const int*);
  void get_raw_data2_(float*,const int*);
  void get_raw_data3_(float*,const int*);
  void get_raw_data4_(float*,const int*);
  void get_raw_data5_(float*,const int*);
  void get_raw_data6_(float*,const int*);
  void plot_(int*);
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
              int nchan, float min, float max, 
              float other=0.) {
    unsigned int dummy(0);
    hbook1_(&id,&name[0],&nchan,(float*)&min,(float*)&max,(float*)&other,dummy);
  }

  void hbook2(int id, std::string name,
              int nchan_x, float x_min, float x_max, 
              int nchan_y, float y_min, float y_max, 
              float other=0.) {
    unsigned int dummy(0);
    hbook2_(&id,&name[0],
            &nchan_x,(float*)&x_min,(float*)&x_max,
            &nchan_y,(float*)&y_min,(float*)&y_max,
            (float*)&other,dummy);
  }
  
  void hbprof(int id, std::string name,
              int nchan_x, float x_min, float x_max,
              float max_value=4000.) {
    float dummy(0);
    float min_value = 0.;
    char opt='S';
    hbprof_(&id, &name[0], &nchan_x, (float*)&x_min,(float*)&x_max, (float*)&min_value,(float*)&max_value, &opt,1,1);
  }

  
  void hf1(int id, float x, float weight=1.0f) {
    hf1_(&id,&x,&weight);
  }
  void hf2(int id, float x, float y, float w=1.0f) {
    hf2_(&id,&x,&y,&w);
  }
  
  void hfill(int id, int istrip, float value,float weight = 1.0f) {
    float fstrip=istrip;
    hfill_(&id,&fstrip,&value,&weight);
  }
  
  void hunpak(int id, float* x, int other=0) {
    char c=' ';
    hunpak_ (&id,x,&c,&other,0);
  }
  void hunpke(int id, float* x, int other=0) {
    char c=' ';
    hunpke_ (&id,x,&c,&other,0);
  }
  void hpak(int id, float* x) {
    char c=' ';
    hpak_ (&id,x);
  }

  void hplot(int id) {
    plot_(&id);
  }
  
}



void prepara_histo(const int N) {

  hbook1(99,"dummy",100,0.,1000.);
  for(int i=0;i<Nsili;++i) {
    // basics
    hbook1(100+i,"ph max",200,0.,2000.);
    hbook1(110+i,"pull",100,0.,250.);

    // ph vs strip
    hbook2(500+i,"ph max strip",100,0.,1000.,384,0,384);
    
    // charge distribution
    hbook1(120+i,"pull_L",100,.0,250.);
    hbook1(130+i,"pull_R",100,.0,250.);
    hbook1(140+i,"eta",n_eta_bin,-1,1.);

    // cluster
    hbook1(150+i,"amplitude",100,0.,1000.);
    hbook1(160+i,"snr",100,0.,500.);
    hbook1(170+i,"nr_strip",5,0.,5.);
    
    hbook1(180+i,"nr_cluster",6,-.5,5.5);

    hbook1(190+i,"x_cm",1000,0.,20000.);
    hbook1(200+i,"interstrip_cm",50,0.,50.);
    hbook1(210+i,"x_pos",1000,0.,20000.);
    hbook1(220+i,"insterstrip_pos",50,0.,50.);
    
  }
  
}



#endif //CERNRUN_H
