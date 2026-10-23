#pragma once
#include "Building.h"
namespace cubbl {
inline BuildingGeometry generatePlatform(){
 BuildingGeometry platform;
 platform.parts.reserve(600);
 auto cube=[&](Vec3 p,Vec3 s,Color c){platform.parts.push_back({p,s,c});};
 const Color concrete={.66f,.65f,.59f},steel={.26f,.34f,.37f},yellow={.96f,.70f,.20f};
 // Ground and foundation tops match the two static physics colliders.
 cube({0,-16.15f,0},{256.f,32.f,24.f},{.48f,.37f,.27f});
 cube({0,-.075f,0},{3.1f,.15f,2.f},concrete);
 const Color paving[]={{.76f,.74f,.66f},{.70f,.70f,.65f},{.81f,.78f,.68f}};
 for(int row=0;row<4;++row)for(int col=0;col<6;++col)
  cube({-1.225f+col*.49f,-.012f,-.69f+row*.46f},{.472f,.024f,.442f},paving[(col+row)%3]);
 for(float z:{-.96f,.96f})for(int i=0;i<12;++i)
  cube({-1.375f+i*.25f,-.008f,z},{.17f,.016f,.07f},i%2?steel:yellow);
 // Subtle patches and repeated tire impressions break up the packed earth.
 const Color dirt[]={{.51f,.40f,.29f},{.53f,.42f,.31f},{.47f,.38f,.29f},{.55f,.44f,.33f}};
 for(int row=0;row<8;++row)for(int col=0;col<18;++col){
  float x=-12.75f+col*1.5f,z=-5.25f+row*1.5f;
  if(std::abs(x)<2 && std::abs(z)<1.7f)continue;
  cube({x,-.148f,z},{1.34f,.004f,1.30f},dirt[(row*7+col*3)%4]);
 }
 for(float x:{-3.3f,-2.6f})for(int i=0;i<24;++i)
  cube({x,-.14f,-4.6f+i*.4f},{.34f,.012f,.13f},{.34f,.29f,.25f});
 for(int i=0;i<8;++i)cube({1.9f+i*.55f,-.12f,1.48f},{.52f,.06f,.70f},paving[i%3]);
 for(int i=0;i<30;++i)cube({3.6f+(i%6)*.22f,-.125f,-1.4f+(i/6)*.25f},{.16f,.05f,.18f},paving[i%3]);
 auto panel=[&](float x,float z,bool side,int index){
  const Color paint=index%2?Color{.27f,.48f,.50f}:Color{.77f,.49f,.27f};
  cube({x,.25f,z},side?Vec3{.06f,.8f,1.02f}:Vec3{1.02f,.8f,.06f},paint);
  for(int rib=-3;rib<=3;++rib)
   cube({x+(side?0:rib*.13f),.25f,z+(side?rib*.13f:0)},side?Vec3{.09f,.76f,.025f}:Vec3{.025f,.76f,.09f},paint);
  cube({x,.25f,z},side?Vec3{.1f,.055f,1.04f}:Vec3{1.04f,.055f,.1f},steel);
 };
 for(float z:{-3.2f,3.2f})for(int i=0;i<13;++i){
  float x=-6.f+i;
  if(z>0 && x>-2 && x<1)continue;
  panel(x,z,false,i);
 }
 for(float x:{-6.5f,6.5f})for(int i=0;i<6;++i)panel(x,-2.7f+i*1.08f,true,i);
 for(float z:{-3.2f,3.2f})for(int i=0;i<14;++i){
  float x=-6.5f+i;
  if(z>0 && x>-1.5f && x<.5f)continue;
  cube({x,.28f,z},{.10f,.86f,.10f},steel);
  cube({x,-.11f,z},{.25f,.08f,.25f},concrete);
 }
 for(float x:{-6.5f,6.5f})for(int i=1;i<6;++i)cube({x,.28f,-3.2f+i*1.0667f},{.1f,.86f,.1f},steel);
 // Striped site sign on the rear fence.
 cube({0,.28f,-3.13f},{1.8f,.45f,.07f},yellow);
 for(int i=0;i<7;++i)cube({-.72f+i*.24f,.28f,-3.085f},{.11f,.35f,.02f},steel);
 for(float x:{-2.f,1.f,4.9f}){
  cube({x,-.11f,2.45f},{.34f,.08f,.34f},steel);
  cube({x,.015f,2.45f},{.22f,.17f,.22f},{.96f,.39f,.12f});
  cube({x,.13f,2.45f},{.15f,.07f,.15f},{.95f,.91f,.77f});
  cube({x,.22f,2.45f},{.10f,.11f,.10f},{.96f,.39f,.12f});
 }
 for(int layer=0;layer<3;++layer)for(int plank=0;plank<5;++plank)
  cube({-4.7f+plank*.22f,-.08f+layer*.12f,-1.6f},{.18f,.09f,1.1f},{.66f,.45f,.26f});
 for(int row=0;row<3;++row)for(int col=0;col<4-row;++col)
  cube({4.7f+col*.27f,-.04f+row*.22f,-2.f},{.25f,.20f,.65f},{.71f,.40f,.27f});
 return platform;
}
}
