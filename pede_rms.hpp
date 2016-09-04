
#include <iostream>
#include <array>
#include <fstream>
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
      //      if(!is.is_good()) return 1;

      for(int isili=0;isili<Nr;++isili) {
        for(int istrip=0;istrip<Nc;++istrip) {
          is >> dummy0 >> dummy1 >> value >> dummy2;
          //          std::cout << value << "\t";
          s[isili][istrip]=static_cast<float>(value);
        }
        // std::cin >> value;
        // std::cout << std::endl;
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
      //      if(!is.is_good()) return 1;

      for(int isili=0;isili<Nr;++isili) {
        for(int istrip=0;istrip<Nc;++istrip) {
          is >> dummy0 >> dummy1 >> dummy2 >> value;
          //          std::cout << value << "\t";
          s[isili][istrip]=static_cast<float>(value);
        }
        // std::cin >> value;
        // std::cout << std::endl;
      }
      return 0;
    }

  };

}
