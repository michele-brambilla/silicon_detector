
#include <iostream>
#include <array>
#include <fstream>
#include <string>

namespace types {
  
  template<int Nr,int Nc>
  struct status {
    
    typedef typename std::vector<bool> array_t; 
    
    static const int Nsili=Nr;
    static const int Nmoduli=Nc;

    std::array<array_t,Nr> s;
    
    status(const int Nstrip=Nc) { 
      for(int i=0;i<Nr;++i) {  s[i].resize(Nstrip); }
    }
    status(const status& other) : s(other.s) { };
    
    array_t& operator[](const int i) { return s[i]; }
    
    template<class T>
    int operator()(const T& pede) {
      if( (T::Nsili != Nsili) || (T::Nmoduli != Nmoduli) || s[0].size() != Nstrip )
        throw std::runtime_error("Errore: dimensioni pede e status non coincidono");

      for(int j=0;j<Nsili;++j) {
        std::fill(s[i].begin(),s[i].end(),0);
        for(int i=0;i<Nstrip;++i)
          if(pede[j][i] < 5.0)
            s[j][i] = 1;
      }
      return 0;
    }

    int operator()(const std::string& filename) { read(filename); }

    int read(const std::string& filename) {
      int value;
      std::ifstream is(filename.c_str());
      //      if(!is.is_good()) return 1;

      for(int istrip=0;istrip<Nc;++istrip) 
        for(int isili=0;isili<Nr;++isili) {
          is >> value;
          s[isili][istrip]=static_cast<bool>(value);
        }
      return 0;
    }

  };


}
