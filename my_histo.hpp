#include <iostream>
#include <ostream>

struct single_histogram {
  single_histogram();
  single_histogram(const single_histogram&);
  single_histogram(const int,
                   const float&,const float&);
  void hf1(float, float);
  void hfill(float, float, float);
  const int n;
  const float low;
  const float high;
  const float delta;
  std::vector<float> v;
};


void hbook1(const int,
            const std::string,
            const int,
            const float&,const float&,
            const int,const int);

void hf1(const int, float, float);
void hfill(const int, float, float, float);
void dump(const int);

single_histogram& get_histogram(const int);








