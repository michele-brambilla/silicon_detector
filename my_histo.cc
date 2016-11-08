#include <iostream>
#include <vector>
#include <map>

#include "my_histo.hpp"

single_histogram::single_histogram() : n(0), low(0), high(0) , delta(0){
  //    std::cout << "called trivial constructor" << std::endl;
};
single_histogram::single_histogram(const single_histogram& other) : 
  n(other.n), low(other.low), high(other.high) , delta(other.delta), 
  v(other.v) { 
  //    std::cout << "called copy constructor" << std::endl;
};
single_histogram::single_histogram(const int nbin,
                                   const float& min,const float& max) : 
  n(nbin), low(min), high(max) , delta((high-low)/n)
{
  //    std::cout << "called hbook constructor" << std::endl;
  v.resize(n);
}
void single_histogram::hf1(float x, float w=1.0) {
  if( x > low && x < high) {
    // std::cout << x-low << std::endl;
    // std::cout << delta << std::endl;      
    int bin = static_cast<int>((x-low)/delta);
    //    std::cout << "fill bin " << bin << std::endl;
    v[bin]+=w;
  }
}
void single_histogram::hfill(float x, float y, float w=1.0) {
  if( x > low && x < high) {
    int bin = static_cast<int>((x-low)/delta);
    v[bin]+=w;
  }
}


std::map<int,single_histogram> histo;

void hbook1(const int id,
            const std::string name,
            const int nbin,
            const float& min,const float& max,
            const int,const int)
{
  // std::cout << id << std::endl;
  single_histogram t(nbin,min,max);
  histo.insert( std::pair<int,single_histogram>(id,t) );
  // std::cout << "insert histo " << id << std::endl;
}

void hf1(const int id, float x, float w=1.0) {
  //  std::cout << id << "\t" << x << std::endl;
  histo[id].hf1(x,w);
}

void hfill(const int id, float x, float y, float w) {
  histo[id].hfill(x,y,w);
}
  
void dump(const int id) {
  for(auto& x : histo[id].v)
    std::cout << "\t" << x << std::endl;
}

single_histogram& get_histogram(const int id) {
  return histo[id];
}
