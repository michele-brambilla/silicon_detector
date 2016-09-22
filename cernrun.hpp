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

  std::vector<std::string> postfix = {"1x","1y","2x","2y"};

  // for(int isili=0;isili<Nsili;++isili) {
  //   // basics

  //   hbook1(100+isili,std::string("ph max")+postfix[isili],200,0.,2000.);
  //   hbook1(110+isili,std::string("pull"  )+postfix[isili],100,0.,250.);

  //   // ph vs strip
  //   hbook2(500+isili,std::string("ph max strip")+postfix[isili],100,0.,1000.,384,0,384);
    
  //   // charge distribution
  //   hbook1(120+isili,std::string("pull_L"  )+postfix[isili],100,.0,250.);
  //   hbook1(130+isili,std::string("pull_R"  )+postfix[isili],100,.0,250.);
  //   hbook1(140+isili,std::string("eta"     )+postfix[isili],n_eta_bin,-1,1.);

  //   // cluster
  //   hbook1(150+isili,std::string("amplitude" )+postfix[isili],100,0.,1000.);
  //   hbook1(160+isili,std::string("snr"       )+postfix[isili],100,0.,500.);
  //   hbook1(170+isili,std::string("nr_strip"  )+postfix[isili],5,0.,5.);
  //   hbook1(180+isili,std::string("nr_cluster")+postfix[isili],6,-.5,5.5);

  //   hbook1(190+isili,std::string("x_cm"             )+postfix[isili],1000,0.,20000.);
  //   hbook1(200+isili,std::string("interstrip_cm"    )+postfix[isili],50,0.,50.);
  //   hbook1(210+isili,std::string("x_pos"            )+postfix[isili],1000,0.,20000.);
  //   hbook1(220+isili,std::string("insterstrip_pos"  )+postfix[isili],50,0.,50.);
    
  // }
  {
    hbook1(100,std::string("ph max")+"1x",200,0.,2000.);
    hbook1(101,std::string("ph max")+"1y",200,0.,2000.);
    hbook1(102,std::string("ph max")+"2x",200,0.,2000.);
    hbook1(103,std::string("ph max")+"2y",200,0.,2000.);
    hbook1(110,std::string("pull"  )+"1x",100,0.,250.);
    hbook1(111,std::string("pull"  )+"1y",100,0.,250.);
    hbook1(112,std::string("pull"  )+"2x",100,0.,250.);
    hbook1(113,std::string("pull"  )+"2y",100,0.,250.);
  }
  {
  hbook1(120,std::string("phL")+"1x",100,0.,250.);
  hbook1(121,std::string("phL")+"1y",100,0.,250.);
  hbook1(122,std::string("phL")+"2x",100,0.,250.);
  hbook1(123,std::string("phL")+"2y",100,0.,250.);
  hbook1(130,std::string("phR")+"1x",100,0.,250.);
  hbook1(131,std::string("phR")+"1y",100,0.,250.);
  hbook1(132,std::string("phR")+"2x",100,0.,250.);
  hbook1(133,std::string("phR")+"2y",100,0.,250.);
  hbook1(140,std::string("eta")+"1x",100,-1.,1.);
  hbook1(141,std::string("eta")+"1y",100,-1.,1.);
  hbook1(142,std::string("eta")+"2x",100,-1.,1.);
  hbook1(143,std::string("eta")+"2y",100,-1.,1.);
  }
  {
  hbook1(150,std::string("amplitude"  )+"1x",100,0.,1000.);
  hbook1(151,std::string("amplitude"  )+"1y",100,0.,1000.);
  hbook1(152,std::string("amplitude"  )+"2x",100,0.,1000.);
  hbook1(153,std::string("amplitude"  )+"2y",100,0.,1000.);
  hbook1(160,std::string("snr"  )+"1x",100,0.,500.);
  hbook1(161,std::string("snr"  )+"1y",100,0.,500.);
  hbook1(162,std::string("snr"  )+"2x",100,0.,500.);
  hbook1(163,std::string("snr"  )+"2y",100,0.,500.);
  hbook1(170,std::string("nr_strip"  )+"1x",5,0.,5.);
  hbook1(171,std::string("nr_strip"  )+"1y",5,0.,5.);
  hbook1(172,std::string("nr_strip"  )+"2x",5,0.,5.);
  hbook1(173,std::string("nr_strip"  )+"2y",5,0.,5.);
  hbook1(180,std::string("nr_cluster"  )+"1x",6,-.5,5.);
  hbook1(181,std::string("nr_cluster"  )+"1y",6,-.5,5.);
  hbook1(182,std::string("nr_cluster"  )+"2x",6,-.5,5.);
  hbook1(183,std::string("nr_cluster"  )+"2y",6,-.5,5.);
  }
  
  int l[1000],n=0;
  hid1_(l,&n);
  for(int i=0;i<n;++i)
    std::cout << l[i] << std::endl;

  std::cin >> n;
}



#endif //CERNRUN_H
