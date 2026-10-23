#pragma once
#include "../game-core/Game.h"
#include <algorithm>
#include <cmath>
namespace cubbl {
inline float viewHalfHeight(float aspect){return std::max(5.8f,3.3f/std::max(.1f,aspect));}
struct CameraFrame { float x=0,y=2.5f,halfHeight=5.8f;bool initialized=false; };
inline void updateCamera(CameraFrame& camera,const GameState& state,float aspect,float dt){
 if(state.phase==GamePhase::Menu){camera={0,2.4f,viewHalfHeight(aspect),true};return;}
 camera.halfHeight=viewHalfHeight(aspect);
 if(camera.initialized && (state.phase==GamePhase::Collapsing || state.phase==GamePhase::Paused || (state.phase==GamePhase::GameOver && state.reason==GameOverReason::Collapse)))return;
 float top=0,x=0;
 for(auto& h:state.tower)if(houseBounds(h.pose).top>top){top=houseBounds(h.pose).top;x=h.pose.centerX;}
 float targetY=top+2.4f;
 if(!camera.initialized || (state.tower.empty() && !state.activeReleased)){camera.x=x;camera.y=targetY;camera.initialized=true;}
 const float follow=1-std::exp(-5*dt);
 camera.x+=(x-camera.x)*follow;camera.y+=(targetY-camera.y)*follow;
}
inline bool houseVisible(HousePose pose,const CameraFrame& camera,float aspect){
 auto b=houseBounds(pose);const float margin=HouseWidth;
 return b.top>=camera.y-camera.halfHeight-margin && b.bottom<=camera.y+camera.halfHeight+margin && b.right>=camera.x-camera.halfHeight*aspect-margin && b.left<=camera.x+camera.halfHeight*aspect+margin;
}
}
