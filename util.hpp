#ifndef _UTIL_H
#define _UTIL_H

#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <iterator>

// helper function to feed int, double etc. to the parameters
template <typename T> 
std::string to_string(const T& x){
  std::stringstream sts;
  sts << x;
  return sts.str();
}
template <typename T> 
std::string to_string(const T& x,int N){
  std::stringstream sts;
  sts << x;
  std::string s(sts.str());
  s.insert(s.begin(), N - s.size(), '0');
  return s;
}

template<class C, int N>
std::ostream& operator<<(std::ostream& os, const std::vector<C> x) {
  std::copy(x.begin(),x.end(), std::ostream_iterator<C>(os, " "));
  os << std::endl;
  return os;
}



std::string read_config_file(const char* s) {
  std::ifstream in;
  try {
    in.open(s);
    in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  }
  catch (std::ifstream::failure e) {
    std::cout << "Exception opening/reading file: " << e.what() << std::endl;
  }
  
  std::string config,buf;
  while(!in.eof()) {
     std::getline(in, buf,'\t');
     config += buf;
  }
  in.close();
  return config;
}

#endif //UTIL_H
