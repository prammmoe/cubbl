#include "game-core/Game.h"
#include "procedural-scene/Building.h"
#include "procedural-scene/City.h"
#include "procedural-scene/Atmosphere.h"
#include "procedural-scene/Platform.h"
#include "native-runtime/FrameClock.h"
#include "native-runtime/Camera.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
using namespace cubbl;
#define CHECK(x) do { if (!(x)) { std::cerr << "Failed line " << __LINE__ << ": " << #x << '\n'; std::exit(1); } } while (0)
void step(Game& g, int count=1) { for(int i=0;i<count;++i) g.update(1.f/60); }
void finishDrop(Game& g) { for(int i=0;i<240 && (g.state().phase==GamePhase::Falling || g.state().phase==GamePhase::Landing || g.state().phase==GamePhase::MissFalling);++i) step(g); }
int main() {
 RunAtmosphere atmosphere(42),sameAtmosphere(42);
 CHECK(atmosphere.current()==TimeOfDay::Sunset);
 bool seen[4]={};
 for(int run=0;run<1000;++run){
  const auto previous=atmosphere.current();
  atmosphere.nextRun();sameAtmosphere.nextRun();
  const auto current=atmosphere.current();
  CHECK(current==sameAtmosphere.current());
  CHECK(run==0 || current!=previous);
  CHECK(static_cast<int>(current)>=0 && static_cast<int>(current)<4);
  seen[static_cast<int>(current)]=true;
  const auto palette=atmospherePalette(current);
  for(auto c:{palette.horizon,palette.middle,palette.zenith,palette.celestial,palette.cloud,palette.worldTint})
   CHECK(std::isfinite(c.r) && std::isfinite(c.g) && std::isfinite(c.b) && c.r>=0 && c.r<=1 && c.g>=0 && c.g<=1 && c.b>=0 && c.b<=1);
  CHECK(palette.celestialX>0 && palette.celestialX<1 && palette.celestialY>0 && palette.celestialY<1);
 }
 for(bool visited:seen)CHECK(visited);
 for(auto time:{TimeOfDay::Morning,TimeOfDay::Day,TimeOfDay::Sunset,TimeOfDay::Night}){
  const auto lighting=atmosphereLighting(time);
  CHECK(std::isfinite(lighting.direction.x) && std::isfinite(lighting.direction.y) && std::isfinite(lighting.direction.z));
  CHECK(lighting.direction.y>0 && lighting.direction.z>0);
  CHECK(lighting.fogDensity>0 && lighting.fogDensity<1 && lighting.glow>=0 && lighting.glow<=1);
  CHECK(lighting.keyColor.r>0 && lighting.keyColor.r<=1 && lighting.keyColor.g>0 && lighting.keyColor.g<=1 && lighting.keyColor.b>0 && lighting.keyColor.b<=1);
 }
 CHECK(atmosphereLighting(TimeOfDay::Night).glow>atmosphereLighting(TimeOfDay::Sunset).glow);
 CHECK(atmosphereLighting(TimeOfDay::Sunset).glow>atmosphereLighting(TimeOfDay::Day).glow);
 CHECK(atmosphereLighting(TimeOfDay::Morning).direction.x<0 && atmosphereLighting(TimeOfDay::Sunset).direction.x>0);
 CHECK(atmospherePalette(TimeOfDay::Day).zenith.b>atmospherePalette(TimeOfDay::Night).zenith.b);
 auto dayCity=generateCity(6,0,TimeOfDay::Day),nightCity=generateCity(6,0,TimeOfDay::Night);
 CHECK(dayCity.parts.size()==nightCity.parts.size());
 int dayLights=0,nightLights=0;
 for(size_t i=0;i<dayCity.parts.size();++i){
  CHECK(dayCity.parts[i].position.x==nightCity.parts[i].position.x && dayCity.parts[i].position.y==nightCity.parts[i].position.y);
  auto a=dayCity.parts[i].color,b=nightCity.parts[i].color;
  if(a.r>.9f && a.g>.65f && a.b<.65f)++dayLights;
  if(b.r>.9f && b.g>.65f && b.b<.65f)++nightLights;
 }
 CHECK(nightLights>dayLights);
 CHECK(classifyPlacement(.12f,2.4f,3.f*Pi/180)==PlacementGrade::Perfect);
 CHECK(classifyPlacement(-.36f,2.4f,8.f*Pi/180)==PlacementGrade::Good);
 CHECK(classifyPlacement(.721f,2.4f)==PlacementGrade::Bad);
 CHECK(classifyPlacement(0,2.4f,4.f*Pi/180)==PlacementGrade::Good);
 CHECK(classifyPlacement(0,2.4f,9.f*Pi/180)==PlacementGrade::Bad);
 CHECK(classifyPlacement(0,0)==PlacementGrade::Miss);
 CHECK(classifyPlacement(std::numeric_limits<float>::quiet_NaN(),2.4f)==PlacementGrade::Miss);
 std::vector<House> straight={{{0,.65f,0},1,1},{{0,1.95f,0},2,2},{{0,3.25f,0},3,3}};
 CHECK(towerImbalance(straight)==0);
 auto messy=straight;messy[1].pose.centerX=.48f;
 CHECK(towerImbalance(messy)>.5f);
 auto tilted=straight;tilted[1].pose.angleRadians=12*Pi/180;
 CHECK(towerImbalance(tilted)>0);
 std::vector<House> leaning={{{0,.65f,0},1,1},{{-.3f,1.95f,0},2,2},{{-.6f,3.25f,0},3,3}};
 auto counterweight=leaning;counterweight.push_back({{-.25f,4.55f,0},4,4});
 auto continuedLean=leaning;continuedLean.push_back({{-.8f,4.55f,0},4,4});
 CHECK(towerImbalance(counterweight)<towerImbalance(leaning));
 CHECK(towerImbalance(counterweight)<towerImbalance(continuedLean));
 CHECK(std::abs(towerSway(4,towerImbalance(counterweight),1,1))<std::abs(towerSway(4,towerImbalance(continuedLean),1,1)));
 auto mirrored=counterweight;for(auto& house:mirrored){house.pose.centerX=-house.pose.centerX;house.pose.angleRadians=-house.pose.angleRadians;}
 CHECK(std::abs(towerImbalance(mirrored)-towerImbalance(counterweight))<1e-6f);
 std::vector<House> overhang={{{0,.65f,0},1,1},{{-1.1f,1.95f,0},2,2},{{1.1f,3.25f,0},3,3}};
 CHECK(towerImbalance(overhang)==1);
 CHECK(towerImbalance({})==0);

 CHECK(towerSway(30,0,1,1)==0 && towerSway(2,1,1,1)==0);
 CHECK(std::abs(towerSway(20,.8f,1,1))>std::abs(towerSway(5,.8f,1,1)));
 CHECK(std::abs(towerSway(10,.8f,1,1))>std::abs(towerSway(10,.8f,1,.2f)));
 CHECK(std::abs(towerSway(1000,1,1,1))<=3.f);
 Stability stability;stability.lastSupportedY=3;
 CHECK(!stability.update({0,3,Pi/4},true,.19f));
 CHECK(!stability.update({0,3,0},true,.1f));
 CHECK(!stability.update({0,3,Pi/4},true,.19f));
 CHECK(stability.update({0,3,Pi/4},true,.02f));
 stability={};stability.lastSupportedY=3;
 CHECK(!stability.update({0,2.34f,0},false,.19f));
 CHECK(stability.update({0,2.34f,0},false,.02f));
 stability={};stability.lastSupportedY=3;
 CHECK(!stability.update({0,2.36f,0},false,.3f));
 Game g;g.dispatch(GameAction::Start);g.dispatch(GameAction::Release);g.dispatch(GameAction::Release);
 while(g.state().phase==GamePhase::Falling)step(g);
 CHECK(g.state().phase==GamePhase::Landing && g.state().score==0);
 g.dispatch(GameAction::Release);step(g,5);CHECK(g.state().score==0);
 g.dispatch(GameAction::Pause);auto y=g.state().active.pose.centerY;
 step(g,200);CHECK(g.state().active.pose.centerY==y);
 g.dispatch(GameAction::Resume);finishDrop(g);
 CHECK(g.state().successfulPlacements==1 && g.state().score==110);
 CHECK(g.takeEvents().size()==1 && g.takeEvents().empty());
 g.dispatch(GameAction::Restart);CHECK(g.state().score==0 && g.state().tower.empty());
 Game stress;stress.dispatch(GameAction::Start);
 for(int i=0;i<10;++i){
  stress.dispatch(GameAction::Release);finishDrop(stress);
  if(stress.state().successfulPlacements!=i+1)std::cerr<<"Stack stopped at "<<i<<" phase "<<int(stress.state().phase)<<"\n";
  CHECK(stress.state().successfulPlacements==i+1);
  CHECK(stress.state().lives==3 && stress.state().reason==GameOverReason::None);
 }
 CHECK(stress.state().score>0 && stress.state().speedMultiplier==1.4f);
 CHECK(stress.takeEvents().size()==10); CHECK(stress.takeEvents().empty());
 CHECK(std::abs(placementSpeed(1)-1.04f)<1e-6f);
 CHECK(placementSpeed(25)==2.f && placementSpeed(50)==3.f);
 CHECK(placementSpeed(100)==3.f && placementSpeed(1000)==3.f);
 CHECK(classifyPlacement(0,HouseWidth,2*Pi)==PlacementGrade::Perfect);
 CHECK(placementReward(PlacementGrade::Perfect,0).points==110);
 CHECK(placementReward(PlacementGrade::Perfect,9).points==200);
 CHECK(placementReward(PlacementGrade::Perfect,10).points==200);
 CHECK(placementReward(PlacementGrade::Good,10).points==60 && placementReward(PlacementGrade::Good,10).combo==0);
 CHECK(placementReward(PlacementGrade::Bad,10).points==30);
 CHECK(placementReward(PlacementGrade::Miss,10).points==0 && placementReward(PlacementGrade::Miss,10).combo==0);
 for(auto& house:stress.state().tower)CHECK(std::abs(house.pose.angleRadians)<3*Pi/180);
 stress.dispatch(GameAction::Pause);auto pose=stress.state().tower.back().pose;
 step(stress,120);CHECK(stress.state().tower.back().pose.centerY==pose.centerY);
 stress.dispatch(GameAction::Restart);CHECK(stress.state().tower.empty());
 for(float aspect:{.33f,.449f,.5625f,1.f,1.8f})
  CHECK(viewHalfHeight(aspect)*aspect >= 1.8f+HouseWidth/2+.1f);
 FrameClock c30,c60,c120;
 int t30=0,t60=0,t120=0;
 for(int i=0;i<30;++i)t30+=c30.advance(1.0/30,true);
 for(int i=0;i<60;++i)t60+=c60.advance(1.0/60,true);
 for(int i=0;i<120;++i)t120+=c120.advance(1.0/120,true);
 CHECK(t30==60 && t60==60 && t120==60);
 CHECK(c30.advance(1000,true)==6);
 CHECK(c30.advance(1000,false)==0);
 c30.reset();CHECK(c30.advance(0,true)==0);
 Game a,b; a.dispatch(GameAction::Start); b.dispatch(GameAction::Start); step(a,120); for(int i=0;i<60;++i){step(b,2);} CHECK(a.state().active.pose.centerX==b.state().active.pose.centerX);
 CHECK(generateBuilding(0,1).parts[0].color.r != generateBuilding(0,2).parts[0].color.r);
 CHECK(generateBuilding(0,1).parts[1].color.r != generateBuilding(0,2).parts[1].color.r);
 for(int variant=0;variant<3;++variant) { auto geometry=generateBuilding(variant,42); auto same=generateBuilding(variant,42); CHECK(geometry.parts.size()==same.parts.size()); CHECK(geometry.parts.size()>=6); for(auto& p:geometry.parts) CHECK(std::isfinite(p.position.x) && p.scale.x>0 && p.scale.y>0 && p.scale.z>0); CHECK(geometry.parts[0].color.r==same.parts[0].color.r); }
 auto platform=generatePlatform();
 CHECK(platform.parts.size()>300 && platform.parts.size()<800);
 bool paved=false,ground=false,fence=false,foundation=false;
 for(const auto& part:platform.parts){
  CHECK(std::isfinite(part.position.x) && std::isfinite(part.position.y) && std::isfinite(part.position.z));
  CHECK(part.scale.x>0 && part.scale.y>0 && part.scale.z>0);
  CHECK(part.color.r>=0 && part.color.r<=1 && part.color.g>=0 && part.color.g<=1 && part.color.b>=0 && part.color.b<=1);
  bool landingArea=std::abs(part.position.x)-part.scale.x/2<HouseWidth/2 && std::abs(part.position.z)-part.scale.z/2<HouseDepth/2;
  if(landingArea)CHECK(part.position.y+part.scale.y/2<=.0001f);
  if(std::abs(part.position.y+part.scale.y/2)<.0001f)paved=true;
  if(part.scale.x==256.f){ground=true;CHECK(std::abs(part.position.y+part.scale.y/2+.15f)<1e-5f);CHECK(part.scale.z==24.f);CHECK(part.position.y-part.scale.y/2 < -viewHalfHeight(.33f));}
  if(part.scale.x==3.1f && part.scale.y==.15f){foundation=true;CHECK(std::abs(part.position.y-part.scale.y/2+.15f)<1e-5f);}
  if(part.scale.y==.8f && (std::abs(part.position.z)==3.2f || std::abs(part.position.x)==6.5f))fence=true;
 }
 CHECK(paved && ground && fence && foundation);
 for(auto time:{TimeOfDay::Morning,TimeOfDay::Day,TimeOfDay::Sunset,TimeOfDay::Night}){
  const auto skyline=generateCity(6,0,time);
  std::vector<Color> distant;
  int glassStrips=0;
  for(const auto& part:skyline.parts){
   if(part.position.z==-15.f && part.scale.x==1.08f && part.scale.z==.9f)distant.push_back(part.color);
   if(part.position.z < -14.f && part.scale.x==.13f && part.scale.z==.035f && part.scale.y>1)++glassStrips;
  }
  CHECK(distant.size()>10 && glassStrips==static_cast<int>(distant.size())*2);
  auto luminance=[](Color c){return .2126f*c.r+.7152f*c.g+.0722f*c.b;};
  for(size_t i=1;i<distant.size();++i)CHECK(std::abs(luminance(distant[i])-luminance(distant[i-1]))>.13f);
 }
 auto city=generateCity(6,0),sameCity=generateCity(6,0),movingCity=generateCity(6,2);
 CHECK(city.parts.size()>200 && city.parts.size()<1500);
 CHECK(city.parts.size()==sameCity.parts.size() && city.parts.size()==movingCity.parts.size());
 bool moved=false,canal=false,quay=false;
 for(size_t i=0;i<city.parts.size();++i){
  const auto& part=city.parts[i];
  CHECK(std::isfinite(part.position.x) && std::isfinite(part.position.y) && part.position.z<-3);
  CHECK(part.scale.x>0 && part.scale.y>0 && part.scale.z>0);
  CHECK(part.color.r>=0 && part.color.r<=1 && part.color.g>=0 && part.color.g<=1 && part.color.b>=0 && part.color.b<=1);
  CHECK(part.position.x==sameCity.parts[i].position.x);
  if(part.position.x!=movingCity.parts[i].position.x)moved=true;
  if(part.scale.y==32.f && part.position.z==-5.25f){
   canal=true;
   const float waterTop=.9848078f*(part.position.y+part.scale.y/2)-.1736482f*.9659258f*(part.position.z+part.scale.z/2);
   const float quayBottom=.9848078f*(-.37f-.65f/2)-.1736482f*.9659258f*(-6.25f+.80f/2);
   CHECK(waterTop>=quayBottom);
   CHECK(part.position.y-part.scale.y/2 < -2*viewHalfHeight(.33f));
  }
  if(part.position.z==-6.25f && part.scale.y==.65f)quay=true;
 }
 for(const auto& part:generateCity(6,1000000.f).parts)CHECK(std::abs(part.position.x)<20.f);
 CHECK(moved && canal && quay && generateCity(0,0).parts.empty());
 CHECK(generateCity(6,std::numeric_limits<float>::infinity()).parts.empty());
 std::cout << "All core and procedural checks passed\n";
}
