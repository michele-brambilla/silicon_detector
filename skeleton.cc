#include<iostream>
#include<fstream>
#include<iterator>
#include<vector>
#include<iomanip>

#include "wrapper.hpp"

void hbook1();


int main(int argc, char **argv) {

  int istat;
  int iev=1,ierr,id=1, run_number=0;
  int output_id=2;

  std::vector<std::string> s;
  s.push_back("run100184_00001.hbook");
  s.push_back("run100184_00002.hbook");
  s.push_back("run100184_00003.hbook");

  // /////////////////////////
  // // Inizializzo il file di output
  init_("result/test.hbook",&output_id);
  // init_( ( std::string(p["output_directory"].GetString())+"/"+
  //          std::string(p["output"].GetString()) ).c_str());

  id = 1000;
  std::string cid = "ph";
  int nbin=100;
  float min = .5;
  float max = 400;
  float dummy = 0.;
  //  hbook1_(&id,&cid[0],&nbin,&min,&max,&dummy,0);
  hbook1(id,cid,nbin,min,max,dummy);
  prepara_histo(6);
  
  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito
  do {
    int nmax=-1;
    
    if(!std::ifstream(s[run_number])) {
      std::cerr << "File " << s[run_number] << " mancante." << std::endl;
      continue;
    }
    
    std::cout << s[run_number] << std::endl;
    s[run_number].resize(s[run_number].size());

    apri_tupla_(s[run_number].c_str(),nmax,40);
    // if(nmax == -1) 
    //   return -1;
    
    std::cout << std::endl;
    std::cout << s[run_number] << std::endl;
    std::cout << "max events = " << nmax << std::endl;
    
    float strip;
    dummy = 1.;
    for(int i = 1;i<10;++i) {
      hfill(id,i,dummy);
    }

    // for(int iev=1;iev<nmax;++iev) {
    //   hgnt_(&id,&iev,&ierr);
    //   if(ierr != 0) 
    //     return ierr;

    // }    

    chiudi_tupla_(40);
    
    //    std::cout << run_number << "\t" <<  atoi(p["nfilemax"].c_str()) << "\t" << istat << std::endl; 
  } while ( ++run_number < 3 );

  
  finalize_(&output_id);

  return 0;
}

