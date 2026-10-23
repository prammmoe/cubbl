#include "game-core/PhysicsWorld.h"
#include "game-core/Game.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#define CHECK(x) do { if(!(x)){std::cerr<<"Physics check "<<__LINE__<<": "<<#x<<'\n';std::exit(1);} }while(0)
using namespace cubbl;
float measureSway(int floors){
 PhysicsWorld driven,quiet;
 for(int i=1;i<=floors;++i){
  HousePose pose{i==1?0.f:(i%2?.24f:-.24f),(i-.5f)*HouseHeight,0};
  driven.add(i,pose);quiet.add(i,pose);
 }
 for(int frame=0;frame<120;++frame){driven.step(1.f/60);quiet.step(1.f/60);}
 float difference=0;
 for(int frame=0;frame<600;++frame){
  std::vector<House> houses;
  for(int i=1;i<=floors;++i)houses.push_back({driven.pose(i),static_cast<uint32_t>(i),static_cast<HouseId>(i)});
  const float error=towerImbalance(houses);
  for(int i=1;i<=floors;++i)driven.applyHorizontalAcceleration(i,towerSway(floors,error,frame/60.f,float(i)/floors));
  driven.step(1.f/60);quiet.step(1.f/60);
  difference=std::max(difference,std::abs(driven.pose(floors).centerX-quiet.pose(floors).centerX));
 }
 return difference;
}
int main(){
 PhysicsWorld world;
 world.add(1,{0,3.25f,0});
 world.step(1.f/60); CHECK(world.pose(1).centerY<3.25f);
 for(int i=0;i<180;++i)world.step(1.f/60);
 CHECK(world.supported(1)); CHECK(world.onTarget(1,0));
 CHECK(std::abs(world.pose(1).centerY-.65f)<.03f);
 world.add(2,{2,3,.3f});
 for(int i=0;i<120;++i)world.step(1.f/60);
 CHECK(std::abs(world.pose(2).angleRadians-.3f)>.05f);
 world.remove(2); CHECK(world.size()==1);
 world.remove(1); CHECK(world.size()==0);
 for(int i=0;i<20;++i){PhysicsWorld clean;clean.add(1,{0,3,0});clean.step(1.f/60);}
 PhysicsWorld ground;
 ground.add(1,{6,3,0});ground.add(2,{6,4.3f,0});
 for(int i=0;i<240;++i)ground.step(1.f/60);
 CHECK(ground.onGround(1) && !ground.supported(1) && !ground.onTarget(1,0));
 CHECK(!ground.onGround(2) && !ground.supported(2) && !ground.onTarget(2,1));
 CHECK(std::abs(ground.pose(1).centerY-.5f)<.03f);
 ground.remove(1);CHECK(!ground.onGround(1));ground.remove(2);CHECK(ground.size()==0);
 CHECK(!world.onGround(1));
 PhysicsWorld tilted;
 const float angle=.2f,extent=houseBounds({0,0,angle}).top;
 tilted.add(1,{1.4f,extent,angle});
 tilted.add(2,{1.4f-std::sin(angle)*HouseHeight,extent+std::cos(angle)*HouseHeight,angle});
 tilted.step(1.f/60);
 CHECK(std::abs(tilted.pose(1).angleRadians)>.05f);
 CHECK(tilted.onTarget(2,1));
 tilted.remove(1);tilted.step(1.f/60);CHECK(!tilted.onTarget(2,1) && !tilted.supported(2));
 PhysicsWorld pushed,untouched;
 pushed.add(1,{0,8,0});untouched.add(1,{0,8,0});
 pushed.applyHorizontalAcceleration(0,100);pushed.applyHorizontalAcceleration(999,100);
 pushed.applyHorizontalAcceleration(1,3);
 pushed.step(1.f/60);untouched.step(1.f/60);
 CHECK(pushed.pose(1).centerX>untouched.pose(1).centerX);
 CHECK(pushed.pose(1).centerY==untouched.pose(1).centerY);
 PhysicsWorld counterbalanced;
 const float offsets[]={0,-.3f,-.6f,-.25f};
 for(HouseId id=1;id<=4;++id){
  const float top=id==1?0:houseBounds(counterbalanced.pose(id-1)).top;
  counterbalanced.add(id,{offsets[id-1],top+2.6f+HouseHeight/2,0});
  float landing=0;
  for(int frame=0;frame<120;++frame){
   counterbalanced.step(1.f/60);
   landing=counterbalanced.onTarget(id,id-1)?landing+1.f/60:0;
  }
  CHECK(landing>=.35f);
 }
 for(int frame=0;frame<300;++frame){
  std::vector<House> houses;
  for(HouseId id=1;id<=4;++id)houses.push_back({counterbalanced.pose(id),id,id});
  const float imbalance=towerImbalance(houses);
  for(HouseId id=1;id<=4;++id)counterbalanced.applyHorizontalAcceleration(id,towerSway(4,imbalance,frame/60.f,id/4.f));
  counterbalanced.step(1.f/60);
  for(HouseId id=1;id<=4;++id){CHECK(counterbalanced.supported(id));CHECK(std::abs(counterbalanced.pose(id).angleRadians)<Pi/4);}
 }
 CHECK(counterbalanced.pose(4).centerX>counterbalanced.pose(3).centerX+.2f);
 PhysicsWorld tower;
 std::vector<Stability> stability(51);
 std::vector<bool> accepted(51,false);
 float swayTime=0;
 auto driveSway=[&](){
  std::vector<House> houses;
  for(HouseId id=1;id<=50;++id)if(accepted[id])houses.push_back({tower.pose(id),id,id});
  const float error=towerImbalance(houses);swayTime+=1.f/60;
  for(size_t i=0;i<houses.size();++i)tower.applyHorizontalAcceleration(houses[i].id,towerSway(static_cast<int>(houses.size()),error,swayTime,float(i+1)/houses.size()));
 };
 for(HouseId n=1;n<=50;++n){
  float top=n==1?0:houseBounds(tower.pose(n-1)).top;
  tower.add(n,{0,top+2.6f+HouseHeight/2,0});
  float landing=0;
  for(int frame=0;frame<90;++frame){
   driveSway();tower.step(1.f/60);
   landing=tower.onTarget(n,n-1)?landing+1.f/60:0;
   if(!accepted[n] && landing+1e-6f>=.35f){accepted[n]=true;stability[n].lastSupportedY=tower.pose(n).centerY;}
   for(HouseId id=1;id<=n;++id)if(accepted[id])CHECK(!stability[id].update(tower.pose(id),tower.supported(id),1.f/60));
  }
  CHECK(accepted[n]);
 }
 for(int frame=0;frame<600;++frame){
  driveSway();tower.step(1.f/60);
  for(HouseId id=1;id<=50;++id)CHECK(!stability[id].update(tower.pose(id),tower.supported(id),1.f/60));
 }
 for(HouseId id=1;id<=50;++id){CHECK(std::abs(tower.pose(id).angleRadians)<3*Pi/180);CHECK(tower.supported(id));}
 const float shortSway=measureSway(8),tallSway=measureSway(16);
 CHECK(shortSway>.01f && tallSway>.10f && tallSway>shortSway*4);
 std::cout<<"Physics adapter checks passed\n";
}
