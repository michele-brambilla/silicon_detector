#include "uparam.hpp"

std::ostream& operator<<(std::ostream& os, const uparam::Param p) {
  for(auto it=p.begin();it!=p.end();++it)
    os <<(*it).first << " : " << (*it).second << std::endl;
  return os;
}
