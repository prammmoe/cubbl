#pragma once
#include <cmath>
#include <cstdint>
namespace cubbl {
constexpr float FoundationWidth=3.1f;
constexpr float HouseWidth=2.4f, HouseHeight=1.3f, HouseDepth=1.4f;
using HouseId=uint32_t;
struct HousePose { float centerX=0, centerY=0, angleRadians=0; };
struct HouseBounds { float left,right,bottom,top; };
inline HouseBounds houseBounds(HousePose p) {
 const float x=(std::abs(std::cos(p.angleRadians))*HouseWidth+std::abs(std::sin(p.angleRadians))*HouseHeight)/2;
 const float y=(std::abs(std::sin(p.angleRadians))*HouseWidth+std::abs(std::cos(p.angleRadians))*HouseHeight)/2;
 return {p.centerX-x,p.centerX+x,p.centerY-y,p.centerY+y};
}
inline float shortestAngle(float angle) { return std::remainder(angle,6.28318530718f); }
}
