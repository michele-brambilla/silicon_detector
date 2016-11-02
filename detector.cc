#include<iostream>
#include<fstream>
#include<sstream>

#include<iterator>
#include<vector>
#include<iomanip>

#include "wrapper.hpp"
#include "input_parser.hpp"


std::string input_file(const rapidjson::Document&, const int);

float get_random_number(float min=0.,float max=1000.) { return rand()/(float)RAND_MAX*(max-min)+min; }


int main(int argc, char **argv) {

  rapidjson::Document p = parse(argc,argv);

  int istat;
  int iev=1,ierr,id=1, run_number=1;

  int output_id=2;
  int tuple_id=40;

  // /////////////////////////
  // // Inizializzo il file di output
  init_( ( std::string(p["output_directory"].GetString())+"/"+
           std::string(p["output"].GetString()) ).c_str(),&output_id);

  prepara_histo(6);

  /////////////////////
  // Esegue l'analisi (finche' ci sono files o fino al limite stabilito
  do {
    int nmax=-1;
    std::string s=input_file(p,run_number);

    if(!std::ifstream(s)) {
      std::cerr << "File " << s << " mancante." << std::endl;
      continue;
    }
    
    std::cout << s << std::endl;
    apri_tupla_(s.c_str(),nmax,tuple_id);
    if(nmax == -1) 
      return -1;
    
    std::cout << std::endl;
    std::cout << s << std::endl;
    std::cout << "max events = " << nmax << std::endl;
    
    float strip;

    for(int iev=1;iev<nmax;++iev) {
      id=1;
      hgnt_(&id,&iev,&ierr);
      if(ierr != 0) 
        return ierr;


    id=100;
    for(int k =0;k<6;++k)
      for(int i = 1;i<10;++i) {
        hfill(id+k,get_random_number(),1.);
      }
      
    }    

    chiudi_tupla_(tuple_id);
    
    //    std::cout << run_number << "\t" <<  atoi(p["nfilemax"].c_str()) << "\t" << istat << std::endl; 
  } while ( ++run_number <= p["nfilemax"].GetInt() );

  
  finalize_(&output_id);

  return 0;
}



std::string input_file(const rapidjson::Document& p, const int run_number) {

  std::ostringstream ss;
  ss << std::setw(5) << std::setfill('0') << run_number;
  std::string s2(ss.str());

  std::string s =  std::string(p["input_directory"].GetString()) + "/run"+std::to_string(p["first"].GetInt())+"_"+s2+".hbook";
  std::cout << s << std::endl;
  s.resize(s.size());

  return s;
}
