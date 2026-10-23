#include "Game.h"
#include <algorithm>
#include <cmath>
#include <utility>
namespace cubbl {
PlacementGrade classifyPlacement(float offset,float width,float angleDifference){
 if(!std::isfinite(offset)||!std::isfinite(width)||!std::isfinite(angleDifference)||width<=0)return PlacementGrade::Miss;
 const float distance=std::abs(offset),angle=std::abs(shortestAngle(angleDifference));
 if(distance<=width*.05f && angle<=3*Pi/180)return PlacementGrade::Perfect;
 if(distance<=width*.15f && angle<=8*Pi/180)return PlacementGrade::Good;
 return PlacementGrade::Bad;
}
float placementSpeed(int successfulPlacements){return std::min(3.f,1.f+.04f*std::max(0,successfulPlacements));}
float towerImbalance(const std::vector<House>& tower){
 double loadX=0;float worst=0;bool misaligned=false;
 for(size_t remaining=tower.size();remaining>0;--remaining){
  const size_t i=remaining-1;
  const auto p=tower[i].pose;
  const HousePose support=i?tower[i-1].pose:HousePose{};
  loadX+=p.centerX;
  const float centerX=static_cast<float>(loadX/(tower.size()-i));
  const float supportX=support.centerX-(i?HouseHeight/2:0)*std::sin(support.angleRadians);
  const float width=std::max(.001f,(i?HouseWidth:FoundationWidth)*std::abs(std::cos(support.angleRadians)));
  const float angle=std::abs(shortestAngle(p.angleRadians-support.angleRadians));
  const float offset=(p.centerX-support.centerX)*std::cos(support.angleRadians)+(p.centerY-support.centerY)*std::sin(support.angleRadians);
  misaligned|=classifyPlacement(offset,HouseWidth,angle)!=PlacementGrade::Perfect;
  const float error=std::max((std::abs(centerX-supportX)/width-.05f)/.25f,(angle-3*Pi/180)/(27*Pi/180));
  worst=std::max(worst,std::clamp(error,0.f,1.f));
 }
 return misaligned?worst:0;
}
float towerSway(int floors,float imbalance,float seconds,float heightFraction){
 if(floors<=2 || !std::isfinite(imbalance) || !std::isfinite(seconds) || !std::isfinite(heightFraction))return 0;
 const float amplitude=std::min(3.f,.10f*(floors-2))*std::clamp(imbalance,0.f,1.f);
 return amplitude*(.3f+.7f*std::clamp(heightFraction,0.f,1.f))*std::sin(seconds*(1.8f+.02f*std::min(floors,30)));
}
PlacementReward placementReward(PlacementGrade grade,int previousCombo){
 if(grade==PlacementGrade::Miss)return {0,0};
 const int combo=grade==PlacementGrade::Perfect?previousCombo+1:0;
 return {(grade==PlacementGrade::Perfect?100:grade==PlacementGrade::Good?60:30)+std::min(100,combo*10),combo};
}
bool Stability::update(HousePose pose,bool supported,float dt){
 if(supported)lastSupportedY=pose.centerY;
 tiltTime=std::abs(shortestAngle(pose.angleRadians))>=Pi/4?tiltTime+dt:0;
 fallTime=!supported && lastSupportedY-pose.centerY>=.65f?fallTime+dt:0;
 return tiltTime+1e-6f>=.2f || fallTime+1e-6f>=.2f;
}
void Game::spawn(){
 target_=0;spawnTop_=0;spawnX_=0;
 for(auto& house:state_.tower){
  float top=houseBounds(house.pose).top;
  if(top>spawnTop_){target_=house.id;spawnTop_=top;spawnX_=house.pose.centerX;}
 }
 state_.phase=GamePhase::Swinging;swingTime_=0;phaseTime_=0;landingTime_=0;
 state_.active={{spawnX_,spawnTop_+2.6f+HouseHeight/2,0},nextId_,nextId_};++nextId_;
 state_.hasActive=true;state_.activeReleased=false;
}
void Game::reset(){
 physics_=std::make_unique<PhysicsWorld>();swayTime_=0;state_={};stability_.clear();events_.clear();nextId_=1;spawn();
}
void Game::end(GameOverReason reason){
 state_.reason=reason;state_.phase=GamePhase::GameOver;
 events_.push_back({EventType::GameOver,PlacementGrade::Miss,state_.score});
}
void Game::collapse(){
 state_.phase=GamePhase::Collapsing;state_.combo=0;phaseTime_=0;
 if(!state_.activeReleased)state_.hasActive=false;
 events_.push_back({EventType::Collapsed,PlacementGrade::Miss,state_.score});
}
void Game::dispatch(GameAction action){
 switch(action){
 case GameAction::Start:if(state_.phase==GamePhase::Menu)reset();break;
 case GameAction::Restart:reset();break;
 case GameAction::Release:
  if(state_.phase==GamePhase::Swinging){physics_->add(state_.active.id,state_.active.pose);state_.activeReleased=true;state_.phase=GamePhase::Falling;}break;
 case GameAction::Pause:
  if(state_.phase!=GamePhase::Menu && state_.phase!=GamePhase::Paused && state_.phase!=GamePhase::GameOver){resumePhase_=state_.phase;state_.phase=GamePhase::Paused;}break;
 case GameAction::Resume:if(state_.phase==GamePhase::Paused)state_.phase=resumePhase_;break;
 case GameAction::ReturnToMenu:state_.phase=GamePhase::Menu;events_.clear();break;
 }
}
void Game::update(float dt){
 if(!std::isfinite(dt)||dt<=0 || state_.phase==GamePhase::Menu || state_.phase==GamePhase::Paused || state_.phase==GamePhase::GameOver)return;
 dt=std::min(dt,.1f);
 if(state_.phase!=GamePhase::Collapsing){
  swayTime_+=dt;
  const float imbalance=towerImbalance(state_.tower);
  const int floors=static_cast<int>(state_.tower.size());
  if(imbalance>0)for(int i=0;i<floors;++i)
   physics_->applyHorizontalAcceleration(state_.tower[i].id,towerSway(floors,imbalance,swayTime_,float(i+1)/floors));
 }
 physics_->step(dt);
 for(auto& house:state_.tower)house.pose=physics_->pose(house.id);
 if(state_.hasActive && state_.activeReleased)state_.active.pose=physics_->pose(state_.active.id);
 if(state_.phase==GamePhase::Collapsing){phaseTime_+=dt;if(phaseTime_+1e-6f>=1.5f)end(GameOverReason::Collapse);return;}
 for(size_t i=0;i<state_.tower.size();++i){
  if(stability_[i].update(state_.tower[i].pose,physics_->supported(state_.tower[i].id),dt)){collapse();return;}
 }
 if(state_.phase==GamePhase::Swinging){
  swingTime_+=dt;state_.active.pose.centerX=spawnX_+1.8f*std::sin(swingTime_*2*Pi/3*state_.speedMultiplier);return;
 }
 if(state_.phase==GamePhase::MissFalling){
  phaseTime_+=dt;
  if(phaseTime_+1e-6f>=.8f){physics_->remove(state_.active.id);state_.hasActive=false;if(state_.lives==0)end(GameOverReason::OutOfLives);else spawn();}
  return;
 }
 const bool supported=physics_->onTarget(state_.active.id,target_);
 landingTime_=supported?landingTime_+dt:0;
 state_.phase=supported?GamePhase::Landing:GamePhase::Falling;
 if(landingTime_+1e-6f>=.35f && !physics_->onGround(state_.active.id)){
  const auto support=physics_->pose(target_);const auto pose=state_.active.pose;
  const float dx=pose.centerX-support.centerX,dy=pose.centerY-support.centerY;
  auto grade=classifyPlacement(dx*std::cos(support.angleRadians)+dy*std::sin(support.angleRadians),HouseWidth,pose.angleRadians-support.angleRadians);
  const auto reward=placementReward(grade,state_.combo);
  state_.combo=reward.combo;state_.score+=reward.points;
  state_.tower.push_back(state_.active);stability_.push_back({pose.centerY,0,0});
  ++state_.successfulPlacements;state_.speedMultiplier=placementSpeed(state_.successfulPlacements);
  events_.push_back({EventType::Landed,grade,state_.score});spawn();
 }else if(physics_->onGround(state_.active.id) || (!supported && houseBounds(state_.active.pose).top<spawnTop_-HouseHeight)){
  --state_.lives;state_.combo=0;phaseTime_=0;state_.phase=GamePhase::MissFalling;
  events_.push_back({EventType::Missed,PlacementGrade::Miss,state_.score});
 }
}
std::vector<GameEvent> Game::takeEvents(){auto result=std::move(events_);events_.clear();return result;}
}
