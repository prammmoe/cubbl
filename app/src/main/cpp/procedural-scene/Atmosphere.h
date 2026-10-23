#pragma once
#include "Building.h"
#include <random>
namespace cubbl {
enum class TimeOfDay { Morning, Day, Sunset, Night };
struct AtmospherePalette {
 Color horizon,middle,zenith,celestial,cloud,worldTint;
 float celestialX,celestialY;
};
inline AtmospherePalette atmospherePalette(TimeOfDay time){
 switch(time){
 case TimeOfDay::Morning:return {{1.f,.81f,.60f},{.73f,.84f,.90f},{.45f,.72f,.88f},{1.f,.92f,.72f},{1.f,.94f,.85f},{1.f,.98f,.94f},.25f,.64f};
 case TimeOfDay::Day:return {{.75f,.90f,.98f},{.41f,.73f,.94f},{.16f,.49f,.78f},{1.f,.98f,.87f},{1.f,1.f,1.f},{1.f,1.f,1.f},.69f,.86f};
 case TimeOfDay::Night:return {{.16f,.20f,.34f},{.08f,.12f,.26f},{.025f,.045f,.12f},{.87f,.93f,1.f},{.30f,.36f,.52f},{.68f,.77f,.96f},.76f,.80f};
 case TimeOfDay::Sunset:default:return {{.99f,.69f,.49f},{.68f,.61f,.79f},{.20f,.39f,.65f},{1.f,.88f,.61f},{1.f,.90f,.78f},{1.f,.94f,.90f},.79f,.70f};
 }
}
struct AtmosphereLighting {
 Vec3 direction;
 Color keyColor;
 float fogDensity,glow;
};
inline AtmosphereLighting atmosphereLighting(TimeOfDay time){
 switch(time){
 case TimeOfDay::Morning:return {{-.65f,.8f,.8f},{1.f,.91f,.77f},.74f,0.f};
 case TimeOfDay::Day:return {{-.3f,1.f,.4f},{1.f,.99f,.93f},.58f,0.f};
 case TimeOfDay::Night:return {{.3f,.7f,1.f},{.62f,.77f,1.f},.64f,1.f};
 case TimeOfDay::Sunset:default:return {{.8f,.35f,.65f},{1.f,.78f,.59f},.72f,.45f};
 }
}
class RunAtmosphere {
public:
 explicit RunAtmosphere(uint32_t seed):random_(seed){}
 TimeOfDay current()const{return current_;}
 void nextRun(){
  int next=std::uniform_int_distribution<int>(0,started_?2:3)(random_);
  if(started_ && next>=static_cast<int>(current_))++next;
  current_=static_cast<TimeOfDay>(next);started_=true;
 }
private:
 std::mt19937 random_;
 TimeOfDay current_=TimeOfDay::Sunset;
 bool started_=false;
};
}
