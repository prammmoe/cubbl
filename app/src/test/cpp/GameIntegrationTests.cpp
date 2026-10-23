#include "game-core/Game.h"
#include "native-runtime/Camera.h"
#include "native-runtime/FrameClock.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace cubbl;
#define CHECK(x) do{if(!(x)){std::cerr<<"Integration line "<<__LINE__<<": "<<#x<<'\n';std::exit(1);}}while(0)
void step(Game& game,int n=1){for(int i=0;i<n;++i)game.update(1.f/60);}
void finish(Game& game){for(int i=0;i<600 && game.state().phase!=GamePhase::Swinging && game.state().phase!=GamePhase::GameOver;++i)step(game);}
Game simulate(int fps){
 Game game;game.dispatch(GameAction::Start);FrameClock clock;
 for(int frame=0;frame<fps*12;++frame)for(int n=clock.advance(1.0/fps,true);n>0;--n){if(game.state().phase==GamePhase::Swinging && game.state().successfulPlacements<10 && (game.state().tower.empty() || game.state().active.pose.centerX-game.state().tower.back().pose.centerX>=.23f))game.dispatch(GameAction::Release);step(game);}
 return game;
}
Game simulateMessy(int fps){
 Game game;game.dispatch(GameAction::Start);FrameClock clock;
 for(int frame=0;frame<fps*15;++frame)for(int n=clock.advance(1.0/fps,true);n>0;--n){
  const auto& state=game.state();
  if(state.phase==GamePhase::Swinging && state.successfulPlacements<10 &&
     (state.tower.empty() || state.active.pose.centerX-state.tower.back().pose.centerX>=.23f))game.dispatch(GameAction::Release);
  step(game);
 }
 return game;
}
void prepareMessy(Game& game){
 game.dispatch(GameAction::Restart);
 for(int tick=0;tick<900 && game.state().successfulPlacements<5;++tick){
  const auto& state=game.state();
  if(state.phase==GamePhase::Swinging && (state.tower.empty() || state.active.pose.centerX-state.tower.back().pose.centerX>=.23f))game.dispatch(GameAction::Release);
  step(game);
 }
 CHECK(game.state().successfulPlacements==5 && towerImbalance(game.state().tower)>.1f);
}
int main(){
 static_assert(static_cast<int>(GamePhase::GameOver)==6 && static_cast<int>(GamePhase::Collapsing)==7);
 static_assert(static_cast<int>(GameAction::ReturnToMenu)==5);
 Game offset;offset.dispatch(GameAction::Start);step(offset,10);offset.dispatch(GameAction::Release);finish(offset);
 CHECK(offset.state().tower.size()==1);CHECK(std::abs(offset.state().tower[0].pose.centerX)>.1f);
 Game miss;miss.dispatch(GameAction::Start);miss.dispatch(GameAction::Release);finish(miss);miss.takeEvents();
 for(int attempt=0;attempt<3;++attempt){
  step(miss,45);miss.dispatch(GameAction::Release);finish(miss);
  CHECK(miss.state().lives==2-attempt && miss.state().successfulPlacements==1);
 }
 CHECK(miss.state().phase==GamePhase::GameOver && miss.state().reason==GameOverReason::OutOfLives);
 int missed=0,over=0;for(auto e:miss.takeEvents()){if(e.type==EventType::Missed)++missed;if(e.type==EventType::GameOver)++over;}
 CHECK(missed==3 && over==1);step(miss,100);CHECK(miss.takeEvents().empty());
 Game groundMiss;groundMiss.dispatch(GameAction::Start);step(groundMiss,45);groundMiss.dispatch(GameAction::Release);
 for(int i=0;i<300 && groundMiss.state().phase!=GamePhase::MissFalling;++i)step(groundMiss);
 CHECK(groundMiss.state().phase==GamePhase::MissFalling && groundMiss.state().lives==2);
 CHECK(groundMiss.state().successfulPlacements==0 && groundMiss.state().score==0);
 auto groundEvents=groundMiss.takeEvents();CHECK(groundEvents.size()==1 && groundEvents[0].type==EventType::Missed);
 auto groundPose=groundMiss.state().active.pose;
 groundMiss.dispatch(GameAction::Pause);step(groundMiss,120);CHECK(groundMiss.state().active.pose.centerY==groundPose.centerY);
 groundMiss.dispatch(GameAction::Resume);step(groundMiss,20);
 CHECK(houseBounds(groundMiss.state().active.pose).bottom>-.19f && groundMiss.takeEvents().empty());
 finish(groundMiss);CHECK(groundMiss.state().phase==GamePhase::Swinging && groundMiss.state().lives==2 && groundMiss.takeEvents().empty());
 Game collapse;collapse.dispatch(GameAction::Start);
 bool sawCollapse=false;
 for(int frame=0;frame<600 && !sawCollapse;++frame){
  const auto& before=collapse.state();
  if(before.phase==GamePhase::Swinging && (before.tower.empty() || before.active.pose.centerX-before.tower.back().pose.centerX>=1.f))collapse.dispatch(GameAction::Release);
  auto score=collapse.state().score;auto lives=collapse.state().lives;step(collapse);
  auto events=collapse.takeEvents();
  for(auto e:events)if(e.type==EventType::Collapsed){
   sawCollapse=true;CHECK(events.size()==1);CHECK(collapse.state().score==score && collapse.state().lives==lives);
  }
 }
 CHECK(sawCollapse && collapse.state().phase==GamePhase::Collapsing);
 CHECK(!collapse.state().hasActive && collapse.state().combo==0 && collapse.state().lives==3);
 const auto score=collapse.state().score;auto pose=collapse.state().tower[0].pose;
 CameraFrame camera;GameState framing=collapse.state();framing.phase=GamePhase::Swinging;updateCamera(camera,framing,.33f,.016f);
 auto frozen=camera;updateCamera(camera,collapse.state(),.33f,.016f);CHECK(camera.x==frozen.x && camera.y==frozen.y && camera.halfHeight==frozen.halfHeight);
 collapse.dispatch(GameAction::Pause);step(collapse,200);CHECK(collapse.state().tower[0].pose.centerY==pose.centerY);
 collapse.dispatch(GameAction::Release);CHECK(collapse.state().phase==GamePhase::Paused);
 collapse.dispatch(GameAction::Resume);collapse.dispatch(GameAction::Release);step(collapse,89);CHECK(collapse.state().phase==GamePhase::Collapsing);
 step(collapse);CHECK(collapse.state().phase==GamePhase::GameOver && collapse.state().reason==GameOverReason::Collapse);
 CHECK(collapse.state().score==score && collapse.state().lives==3);CHECK(collapse.takeEvents().size()==1);step(collapse,100);CHECK(collapse.takeEvents().empty());
 for(int i=0;i<100;++i){collapse.dispatch(GameAction::Restart);collapse.dispatch(GameAction::Release);step(collapse,2);CHECK(collapse.state().tower.empty() && collapse.state().lives==3);}
 auto a=simulate(30),b=simulate(60),c=simulate(120);
 CHECK(a.state().reason==GameOverReason::Collapse && a.state().score==b.state().score && b.state().score==c.state().score);
 CHECK(a.state().tower.size()==b.state().tower.size() && b.state().tower.size()==c.state().tower.size());
 for(size_t i=0;i<a.state().tower.size();++i){CHECK(std::abs(a.state().tower[i].pose.centerY-b.state().tower[i].pose.centerY)<1e-5f);CHECK(std::abs(b.state().tower[i].pose.angleRadians-c.state().tower[i].pose.angleRadians)<1e-5f);}
 Game paused,reference;paused.dispatch(GameAction::Start);reference.dispatch(GameAction::Start);paused.dispatch(GameAction::Release);reference.dispatch(GameAction::Release);step(paused,15);step(reference,15);
 paused.dispatch(GameAction::Pause);step(paused,200);paused.dispatch(GameAction::Resume);step(paused,10);step(reference,10);CHECK(paused.state().active.pose.centerY==reference.state().active.pose.centerY);
 auto m30=simulateMessy(30),m60=simulateMessy(60),m120=simulateMessy(120);
 CHECK(m30.state().successfulPlacements>=3 && m30.state().reason==GameOverReason::Collapse);
 CHECK(m30.state().score==m60.state().score && m60.state().score==m120.state().score);
 CHECK(m30.state().tower.size()==m60.state().tower.size() && m60.state().tower.size()==m120.state().tower.size());
 for(size_t i=0;i<m30.state().tower.size();++i){
  CHECK(std::abs(m30.state().tower[i].pose.centerX-m60.state().tower[i].pose.centerX)<1e-5f);
  CHECK(std::abs(m60.state().tower[i].pose.angleRadians-m120.state().tower[i].pose.angleRadians)<1e-5f);
 }
 Game swayPaused,swayReference;prepareMessy(swayPaused);prepareMessy(swayReference);
 auto heldPose=swayPaused.state().tower.back().pose;
 swayPaused.dispatch(GameAction::Pause);step(swayPaused,200);CHECK(swayPaused.state().tower.back().pose.centerX==heldPose.centerX);
 swayPaused.dispatch(GameAction::Resume);step(swayPaused,120);step(swayReference,120);
 CHECK(swayPaused.state().tower.back().pose.centerX==swayReference.state().tower.back().pose.centerX);
 CHECK(swayPaused.state().phase==swayReference.state().phase);
 Game fresh;prepareMessy(swayPaused);prepareMessy(fresh);step(swayPaused,60);step(fresh,60);
 CHECK(swayPaused.state().tower.back().pose.centerX==fresh.state().tower.back().pose.centerX);
 auto bounds=houseBounds({2,3,Pi/2});CHECK(std::abs(bounds.right-(2+HouseHeight/2))<1e-5f);CHECK(std::abs(bounds.top-(3+HouseWidth/2))<1e-5f);
 GameState shifted;shifted.phase=GamePhase::Swinging;shifted.hasActive=true;shifted.active={ {9,15,Pi/4},1,1};shifted.tower.push_back({{8,11,Pi/4},2,2});
 for(float aspect:{.33f,.449f,.5625f}){CameraFrame view;updateCamera(view,shifted,aspect,.016f);CHECK(houseVisible(shifted.active.pose,view,aspect));CHECK(houseVisible(shifted.tower[0].pose,view,aspect));CHECK(!houseVisible({100,100,0},view,aspect));auto active=houseBounds(shifted.active.pose);CHECK(active.right-view.x<view.halfHeight*aspect);CHECK(active.top-view.y<view.halfHeight*.8f);}
 for(float aspect:{.33f,.449f,.5625f}){
  CameraFrame view;
  for(int frame=0;frame<300;++frame){
   shifted.tower[0].pose={frame*.02f,11+frame*.03f,std::sin(frame*.02f)*.4f};
   shifted.active.pose={shifted.tower[0].pose.centerX+1.8f*std::sin(frame*.1f),shifted.tower[0].pose.centerY+4,0};
   updateCamera(view,shifted,aspect,1.f/60);
   CHECK(view.halfHeight==viewHalfHeight(aspect));
   CHECK(houseVisible(shifted.active.pose,view,aspect));
  }
  shifted.phase=GamePhase::Paused;auto held=view;updateCamera(view,shifted,aspect,1);
  CHECK(view.x==held.x && view.y==held.y && view.halfHeight==held.halfHeight);
  updateCamera(view,shifted,.33f,1);CHECK(view.halfHeight==viewHalfHeight(.33f));
  shifted.phase=GamePhase::Swinging;
 }
 shifted.phase=GamePhase::Menu;CameraFrame menuCamera{99,99,99,true};updateCamera(menuCamera,shifted,.33f,.016f);CHECK(menuCamera.x==0 && menuCamera.y==2.4f);
 std::cout<<"Game endings, input, pause, restart, camera and frame-rate checks passed\n";
}
