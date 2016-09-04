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

#endif //UTIL_H
