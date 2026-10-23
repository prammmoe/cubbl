#pragma once
#include <algorithm>
#include <cmath>
namespace cubbl {
class FrameClock {
public:
 int advance(double elapsed,bool running) {
  if(!running){reset();return 0;}
  if(!std::isfinite(elapsed)||elapsed<0)return 0;
  accumulator_+=std::min(.1,elapsed);
  int count=0;
  while(accumulator_+1e-10>=1.0/60 && count<6){accumulator_-=1.0/60;++count;}
  accumulator_=std::max(0.0,accumulator_);
  return count;
 }
 void reset(){accumulator_=0;}
private:
 double accumulator_=0;
};
}
