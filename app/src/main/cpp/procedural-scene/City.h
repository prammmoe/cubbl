#pragma once
#include "Building.h"
#include "Atmosphere.h"
#include <algorithm>
#include <cmath>
namespace cubbl {
// Local city coordinates: y=0 at street level, negative z behind the tower.
inline BuildingGeometry generateCity(float halfWidth,float seconds,TimeOfDay time=TimeOfDay::Sunset){
 BuildingGeometry city;
 if(!std::isfinite(halfWidth)||!std::isfinite(seconds)||halfWidth<=0)return city;
 halfWidth=std::min(halfWidth,30.f);
 auto cube=[&](Vec3 p,Vec3 s,Color c){city.parts.push_back({p,s,c});};
 const Color facades[]={{.82f,.38f,.30f},{.24f,.52f,.56f},{.64f,.40f,.57f},{.83f,.61f,.30f},{.38f,.43f,.65f},{.52f,.66f,.56f}};
 const int count=static_cast<int>(std::ceil(halfWidth/1.35f))+3;
 city.parts.reserve(static_cast<size_t>(2*count+1)*48+32);
 for(int layer=0;layer<3;++layer){
  float z=-15.f+layer*3.3f;
  for(int i=-count;i<=count;++i){
   const unsigned seed=static_cast<unsigned>(i+128)*1664525u+static_cast<unsigned>(layer)*1013904223u;
   float height=1.2f+float(seed%11)*.23f+(2-layer)*.35f;
   float x=i*1.35f+(layer%2)*.6f;
   Color color=facades[(i+128+layer*2)%6];
   if(layer==0){
    const float value=(i+128)%2==0?.82f:1.10f;
    color={(color.r*.35f+.46f*.65f)*value,(color.g*.35f+.57f*.65f)*value,(color.b*.35f+.72f*.65f)*value};
   }else if(layer==1)color={color.r*.78f+.46f*.22f,color.g*.78f+.57f*.22f,color.b*.78f+.72f*.22f};
   cube({x,height/2,z},{1.08f,height,.9f},color);
   cube({x,height+.06f,z},{1.19f,.12f,1.02f},{color.r*.75f,color.g*.75f,color.b*.8f});
   if(seed%3==0)cube({x+.22f,height+.24f,z},{.27f,.32f,.35f},{.35f,.39f,.49f});
   cube({x,height-.04f,z+.47f},{1.10f,.045f,.035f},{color.r*.7f+.3f,color.g*.7f+.3f,color.b*.7f+.3f});
   if(layer==0){
    for(float offset:{-.25f,.25f})cube({x+offset,height/2,z+.465f},{.13f,height-.35f,.035f},{color.r*.60f,color.g*.67f,color.b*.78f});
    continue;
   }
   for(int row=0;row<static_cast<int>(height/.48f);++row)for(int col=0;col<2;++col){
    const bool afterDark=time==TimeOfDay::Night || time==TimeOfDay::Sunset;
    const bool lit=(seed+row*7+col*3)%5<(time==TimeOfDay::Night?4:3);
    const Color window=lit?(afterDark?Color{1.f,.79f,.39f}:Color{.47f,.69f,.78f}):Color{.19f,.30f,.42f};
    cube({x+(col? .27f:-.27f),.30f+row*.48f,z+.465f},{.23f,.26f,.035f},window);
    if(layer==2)cube({x+(col?.27f:-.27f),.15f+row*.48f,z+.49f},{.31f,.04f,.09f},{.91f,.76f,.60f});
   }
   if(layer==2){
    cube({x,.20f,z+.48f},{.20f,.4f,.04f},{.18f,.27f,.35f});
    if(seed%2==0){
     cube({x,.53f,z+.66f},{.9f,.13f,.38f},{.92f,.47f,.33f});
     for(int stripe=0;stripe<3;++stripe)cube({x-.32f+stripe*.32f,.605f,z+.66f},{.12f,.025f,.38f},{1.f,.86f,.63f});
    }
   }
  }
 }
 cube({0,-.18f,-9.4f},{halfWidth*2+8,.30f,3.8f},{.36f,.41f,.51f});
 cube({0,-.46f,-9.4f},{halfWidth*2+8,.28f,3.7f},{.24f,.29f,.39f});
 cube({0,.015f,-6.65f},{halfWidth*2+8,.12f,.30f},{.80f,.70f,.58f});
 cube({0,-.035f,-7.4f},{halfWidth*2+8,.09f,1.25f},{.25f,.29f,.37f});
 cube({0,.03f,-8.1f},{halfWidth*2+8,.10f,.22f},{.83f,.68f,.54f});
 for(int i=-count;i<=count;++i){
  for(int brick=0;brick<2;++brick){
   const Color stone=(i+brick)%3==0?Color{.65f,.42f,.34f}:Color{.62f,.62f,.66f};
   cube({i*1.35f-.33f+brick*.66f,-.20f,-6.72f},{.63f,.22f,.07f},stone);
  }
  cube({i*1.35f,.025f,-7.3f},{.55f,.018f,.055f},{.94f,.76f,.44f});
  if(i%2==0){
   float x=i*1.35f+.4f;
   cube({x,.35f,-6.8f},{.09f,.75f,.09f},{.40f,.27f,.24f});
   cube({x,.91f,-6.8f},{.55f,.64f,.50f},{.20f,.46f,.39f});
   cube({x-.14f,1.12f,-6.8f},{.40f,.38f,.39f},{.33f,.59f,.41f});
   cube({x+.7f,.49f,-7.8f},{.035f,1.f,.035f},{.29f,.32f,.43f});
   cube({x+.7f,1.03f,-7.8f},{.15f,.13f,.15f},{1.f,.80f,.41f});
  }
 }
 // The canal continues below the camera-relative quay, behind the playable tower.
 const float canalWidth=halfWidth*2+8;
 cube({0,-16.48f,-5.25f},{canalWidth,32.f,.12f},{.16f,.43f,.51f});
 cube({0,-.37f,-6.25f},{canalWidth,.65f,.80f},{.40f,.45f,.49f});
 cube({0,-.06f,-5.88f},{canalWidth,.14f,.22f},{.82f,.76f,.63f});
 for(int i=-count;i<=count;++i){
  cube({i*1.35f,-.38f,-5.83f},{1.28f,.24f,.06f},i%2?Color{.57f,.60f,.60f}:Color{.66f,.63f,.56f});
  cube({i*1.35f+.65f,-.58f,-5.83f},{1.28f,.12f,.06f},{.43f,.48f,.50f});
 }
 for(int row=0;row<18;++row)for(int col=-count;col<=count;++col){
  const float drift=.18f*std::sin(seconds*.65f+row*.9f);
  const float x=col*1.35f+(row%2)*.5f+drift;
  const float y=-.78f-row*.72f;
  const Color shimmer=(col+row)%3==0?Color{.48f,.72f,.72f}:Color{.22f,.53f,.59f};
  cube({x,y,-5.16f},{.32f+((col+count)%3)*.16f,.025f,.018f},shimmer);
 }
 for(int boat=0;boat<2;++boat){
  const float span=canalWidth-2;
  const float phase=seconds*(boat?-.23f:.28f)+boat*span/2;
  const float x=phase-std::floor(phase/span)*span-span/2;
  const float y=-.95f-boat*2.1f;
  cube({x,y,-4.95f},{1.10f,.20f,.36f},boat?Color{.76f,.38f,.26f}:Color{.89f,.77f,.53f});
  cube({x,y+.13f,-4.95f},{.82f,.07f,.34f},{.91f,.87f,.72f});
  cube({x-.10f,y+.29f,-4.95f},{.42f,.25f,.30f},{.82f,.87f,.82f});
  cube({x-.10f,y+.30f,-4.78f},{.26f,.12f,.02f},{.19f,.36f,.44f});
  cube({x-.10f,y+.44f,-4.95f},{.52f,.06f,.38f},{.30f,.40f,.46f});
  cube({x,y-.15f,-5.14f},{1.36f,.025f,.018f},{.52f,.75f,.74f});
 }
 const float span=halfWidth*2+8;
 for(int i=0;i<4;++i){
  const float phase=seconds*(i%2?-.42f:.55f)+i*span/4;
  float x=phase-std::floor(phase/span)*span-span/2;
  float z=i%2?-7.6f:-7.1f;
  Color color=facades[(i+3)%6];
  cube({x,.12f,z},{.56f,.20f,.27f},color);
  cube({x-.04f,.27f,z},{.30f,.15f,.23f},{.65f,.79f,.82f});
  for(float side:{-.18f,.18f})cube({x+side,.04f,z+.14f},{.12f,.12f,.035f},{.15f,.20f,.28f});
  cube({x+.29f,.13f,z},{.025f,.07f,.20f},{1.f,.86f,.55f});
 }
 return city;
}
}
