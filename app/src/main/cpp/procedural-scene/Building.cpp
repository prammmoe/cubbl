#include "Building.h"
#include "../game-core/Game.h"
namespace cubbl {
BuildingGeometry generateBuilding(int variant,uint32_t seed) {
 constexpr Color palettes[]={{.96f,.66f,.65f},{.72f,.68f,.87f},{.68f,.82f,.69f},{.98f,.79f,.54f},{.63f,.79f,.86f}};
 constexpr Color roofs[]={{.64f,.32f,.28f},{.30f,.39f,.58f},{.72f,.39f,.28f},{.28f,.47f,.49f},{.45f,.34f,.53f}};
 const Color body=palettes[seed%5],roof=roofs[seed%5];
 const Color cream={1.f,.95f,.83f}, glass={.20f,.42f,.48f};
 BuildingGeometry result;
 auto cube=[&](Vec3 p,Vec3 s,Color c){result.parts.push_back({p,s,c});};
 cube({0,HouseHeight/2,0},{HouseWidth,HouseHeight,HouseDepth},body);
 cube({0,1.27f,0},{2.5f,.10f,1.5f},roof);
 cube({0,.07f,0},{2.45f,.14f,1.45f},cream);
 variant=(variant%3+3)%3;
 for(int row=0;row<(variant==2?2:1);++row) for(int col=0;col<2;++col) {
  float x=col==0?-.68f:.68f, y=variant==2?.43f+row*.46f:.76f;
  cube({x,y,.716f},{.54f,.36f,.06f},cream);
  cube({x,y,.755f},{.40f,.25f,.035f},glass);
 }
 if(variant!=2) cube({0,.38f,.72f},{.35f,.61f,.08f},glass);
 for(int row=0;row<(variant==2?2:1);++row){
  float y=variant==2?.43f+row*.46f:.76f;
  cube({1.215f,y,0},{.045f,.36f,.56f},cream);
  cube({1.245f,y,0},{.02f,.25f,.40f},glass);
 }
 if(variant==1){
  cube({0,1.02f,.84f},{2.25f,.10f,.38f},roof);
  for(int stripe=0;stripe<5;++stripe)cube({-.9f+stripe*.45f,1.076f,.84f},{.18f,.012f,.38f},cream);
 }
 return result;
}
}
