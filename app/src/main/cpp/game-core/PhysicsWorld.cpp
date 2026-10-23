#include "PhysicsWorld.h"
#include <box2d/box2d.h>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
namespace cubbl {
struct PhysicsWorld::Impl {
 b2WorldId world;
 b2BodyId ground;
 std::set<HouseId> groundContacts;
 std::map<HouseId,b2BodyId> bodies;
 std::map<HouseId,std::set<HouseId>> supports;
 std::set<HouseId> grounded;
 Impl(){
  auto def=b2DefaultWorldDef();def.gravity={0,-12};def.contactDampingRatio=2.0f;world=b2CreateWorld(&def);
  auto body=b2DefaultBodyDef();body.position={0,-.075f};
  auto foundation=b2CreateBody(world,&body);bodies[0]=foundation;
  auto shape=b2DefaultShapeDef();shape.material.friction=.8f;shape.material.restitution=0;
  auto box=b2MakeBox(FoundationWidth/2,.075f);b2CreatePolygonShape(foundation,&shape,&box);
  body.position={0,-.25f};ground=b2CreateBody(world,&body);
  box=b2MakeBox(128.f,.1f);b2CreatePolygonShape(ground,&shape,&box);
 }
 ~Impl(){b2DestroyWorld(world);}
};
PhysicsWorld::PhysicsWorld():impl_(std::make_unique<Impl>()){}
PhysicsWorld::~PhysicsWorld()=default;
void PhysicsWorld::add(HouseId id,HousePose p){
 if(!id || impl_->bodies.count(id) || !std::isfinite(p.centerX)||!std::isfinite(p.centerY)||!std::isfinite(p.angleRadians))throw std::invalid_argument("Invalid house body");
 auto def=b2DefaultBodyDef();def.type=b2_dynamicBody;def.position={p.centerX,p.centerY};def.rotation=b2MakeRot(p.angleRadians);

 // Let resting islands sleep before accumulated solver jitter disturbs tall centered stacks.
 def.sleepThreshold=1.f;
 def.userData=reinterpret_cast<void*>(static_cast<uintptr_t>(id));
 auto body=b2CreateBody(impl_->world,&def);impl_->bodies[id]=body;
 auto shape=b2DefaultShapeDef();shape.density=1;shape.material.friction=.8f;shape.material.restitution=0;
 auto box=b2MakeBox(HouseWidth/2,HouseHeight/2);b2CreatePolygonShape(body,&shape,&box);
}
void PhysicsWorld::remove(HouseId id){
 auto it=impl_->bodies.find(id);if(!id||it==impl_->bodies.end())return;
 b2DestroyBody(it->second);impl_->bodies.erase(it);impl_->supports.erase(id);impl_->grounded.erase(id);impl_->groundContacts.erase(id);
 for(auto& item:impl_->supports)item.second.erase(id);
}
HousePose PhysicsWorld::pose(HouseId id)const{
 auto body=impl_->bodies.at(id);auto p=b2Body_GetPosition(body);return {p.x,p.y,b2Rot_GetAngle(b2Body_GetRotation(body))};
}
size_t PhysicsWorld::size()const{return impl_->bodies.size()-1;}
bool PhysicsWorld::onGround(HouseId id)const{return impl_->groundContacts.count(id)>0;}
bool PhysicsWorld::supported(HouseId id)const{return impl_->grounded.count(id)>0;}
bool PhysicsWorld::onTarget(HouseId id,HouseId target)const{
 auto it=impl_->supports.find(id);return supported(target)&&it!=impl_->supports.end()&&it->second.count(target)>0;
}
void PhysicsWorld::applyHorizontalAcceleration(HouseId id,float acceleration){
 if(!id || !std::isfinite(acceleration) || acceleration==0)return;
 auto it=impl_->bodies.find(id);if(it==impl_->bodies.end())return;
 auto body=it->second;
 b2Body_ApplyForceToCenter(body,{b2Body_GetMass(body)*acceleration,0},true);
}
void PhysicsWorld::step(float dt){
 if(!std::isfinite(dt)||dt<=0)return;
 b2World_Step(impl_->world,dt,4);
 impl_->supports.clear();impl_->grounded={0};impl_->groundContacts.clear();
 std::vector<b2ContactData> contacts;
 for(auto& item:impl_->bodies){
  contacts.resize(b2Body_GetContactCapacity(item.second));
  int count=b2Body_GetContactData(item.second,contacts.data(),static_cast<int>(contacts.size()));
  for(int i=0;i<count;++i){
   auto& c=contacts[i];bool touching=false;
   for(int j=0;j<c.manifold.pointCount;++j)if(c.manifold.points[j].separation<=.005f)touching=true;
   if(!touching)continue;
   auto a=static_cast<HouseId>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(b2Shape_GetBody(c.shapeIdA))));
   auto b=static_cast<HouseId>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(b2Shape_GetBody(c.shapeIdB))));
   const bool groundA=B2_ID_EQUALS(b2Shape_GetBody(c.shapeIdA),impl_->ground);
   const bool groundB=B2_ID_EQUALS(b2Shape_GetBody(c.shapeIdB),impl_->ground);
   if(groundA || groundB){
    if(groundA && c.manifold.normal.y>=.5f)impl_->groundContacts.insert(b);
    if(groundB && c.manifold.normal.y<=-.5f)impl_->groundContacts.insert(a);
    continue;
   }
   if(c.manifold.normal.y>=.5f)impl_->supports[b].insert(a);
   if(c.manifold.normal.y<=-.5f)impl_->supports[a].insert(b);
  }
 }
 std::vector<HouseId> queue{0};
 std::map<HouseId,std::vector<HouseId>> above;
 for(auto& edge:impl_->supports)for(auto below:edge.second)above[below].push_back(edge.first);
 for(size_t i=0;i<queue.size();++i)for(auto id:above[queue[i]])if(impl_->grounded.insert(id).second)queue.push_back(id);
}
}
