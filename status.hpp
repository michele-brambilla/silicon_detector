#include <iostream>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>



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


    int operator()(const std::string& filename) { read(filename); }

    int read(const std::string& filename) {
      int value;
      std::ifstream is(filename.c_str());
      if( !is.good() )
        throw std::runtime_error("Impossibile aprere il file di stato");

      for(int istrip=0;istrip<Nc;++istrip) 
        for(int isili=0;isili<Nr;++isili) {
          is >> value;
          s[isili][istrip]=static_cast<bool>(value);
        }
      return 0;
    }

    template <typename T>
    int set( T& p) {
      if( (T::Nsili != Nsili) ||
          (p[0].size() != s[0].size() ) )
        throw std::runtime_error("Le dimensioni di status e pede non coincidono.");
      
      for(int isili=0;isili<Nr;++isili) {
        fill(s[isili].begin(),s[isili].end(),false);
        for(int istrip=0;istrip<Nc;++istrip) 
          if( p[isili][istrip] < 1 )
            s[isili][istrip] = true;
      }
      return 0;
    }

  };


}

template<int Nsili,int Nstrip>
void read_status(types::status<Nsili,Nstrip>& status, const std::string& s) {

  std::ifstream is(s);
  int istrip=0;

  if( !is.good() )
    throw std::runtime_error("Impossibile aprere il file di stato");

  std::string line;
  while(is.good() && (istrip < Nstrip) ) {
    std::getline(is,line);
    std::istringstream ss(line);
    std::vector <float> record;
    while (ss) {
      std::string item;
      if (!std::getline( ss, item, '\t' )) break;
      record.push_back(std::stoi(item));
    }
    for(int isili=0;isili<Nsili;++isili)
      status[isili][istrip] = record[isili];
    istrip++;
  }
  is.close();
}

