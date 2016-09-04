
#include <iostream>
#include <fstream>
#include <sstream>

#include "pedestal.hpp"
#include "types.hpp"

template<class data_t>
int leggi_pede(const std::string& in, std::vector<data_t>& pede) {
  std::ifstream file(in.c_str());
  float dummy1,dummy2,dummy3;

  for(typename std::vector<data_t>::iterator p=pede.begin();p!=pede.end();++p) {

    typename data_t::iterator it=(*p).begin();
    do {
      file >> dummy1 >> dummy2 >> (*it) >> dummy3;
    } while( (++it) != (*p).end() );
    
  }

  file.close();
  return 0;
}



template<class data_t>
int leggi_rms(const std::string& in, std::vector<data_t>& rms) {
  std::ifstream file(in.c_str());
  float dummy1,dummy2,dummy3;

  for(typename std::vector<data_t>::iterator p=rms.begin();p!=rms.end();++p) {

    typename data_t::iterator it=(*p).begin();
    do {
      file >> dummy1 >> dummy2 >> dummy3 >> (*it);
    } while( (++it) != (*p).end() );
    
  }

  file.close();
  return 0;
}

