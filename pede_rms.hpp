
#include <iostream>
#include <array>
#include <fstream>
#include <sstream>
#include <string>

namespace types {
  
  template<int Nr,int Nc>
  struct pedestal {
    
    typedef typename std::vector<float> array_t; 
    
    static const int Nsili=Nr;
    static const int Nmoduli=Nc;

    std::array<array_t,Nr> s;
    
    pedestal(const int Nstrip=Nc) { 
      for(int i=0;i<Nr;++i) {  s[i].resize(Nstrip); }
    }
    pedestal(const pedestal& other) : s(other.s) { };
    
    array_t& operator[](const int i) { return s[i]; }
    
    int operator()(const std::string& filename) { read(filename); }

    int read(const std::string& filename) {
      float dummy0,dummy1,dummy2;
      float value;
      std::ifstream is(filename.c_str());
      if(!is.good() )
        throw std::runtime_error("Impossibile aprire il file di pede\n");
       
      for(int isili=0;isili<Nr;++isili) {
        for(int istrip=0;istrip<Nc;++istrip) {
          is >> dummy0 >> dummy1 >> value >> dummy2;
          s[isili][istrip]=static_cast<float>(value);
        }
      }
      return 0;
    }

  };


  template<int Nr,int Nc>
  struct rms {
    
    typedef typename std::vector<float> array_t; 
    
    static const int Nsili=Nr;
    static const int Nmoduli=Nc;

    std::array<array_t,Nr> s;
    
    rms(const int Nstrip=Nc) { 
      for(int i=0;i<Nr;++i) {  s[i].resize(Nstrip); }
    }
    rms(const rms& other) : s(other.s) { };
    
    array_t& operator[](const int i) { return s[i]; }
    
    int operator()(const std::string& filename) { read(filename); }

    int read(const std::string& filename) {
      float dummy0,dummy1,dummy2;
      float value;
      std::ifstream is(filename.c_str());
      if(!is.good() )
        throw std::runtime_error("Impossibile aprire il file di rms\n");

      for(int isili=0;isili<Nr;++isili) {
        for(int istrip=0;istrip<Nc;++istrip) {
          is >> dummy0 >> dummy1 >> dummy2 >> value;
          s[isili][istrip]=static_cast<float>(value);
        }
      }
      return 0;
    }

  };

}


template<typename detector_t>
void read_pede(detector_t& sili, const std::string& s) {
  int Nsili = sili.size();
  int Nstrip = sili[0].value.size();

  std::ifstream is;
  is.open(s);
  int c=0,value,dummy;
  if( !is.good() )
    throw std::runtime_error("Impossibile aprere il file di stato");
  
  std::string line;  
  while(is.good() && (c/Nstrip < Nsili) ) {
    std::getline(is,line);
    std::istringstream ss(line);
    std::vector <float> record;
    while (ss) {
      std::string item;

      if (!std::getline( ss, item, '\t' )) break;
      record.push_back(std::stof(item));
    }
    sili[c/Nstrip].spede[c%Nstrip] = record[2];
    sili[c/Nstrip].srms[c%Nstrip]  = record[3];
    c++;
  }
  
  is.close();
}
