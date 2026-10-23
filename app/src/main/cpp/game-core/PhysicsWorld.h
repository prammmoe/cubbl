#pragma once
#include "HousePose.h"
#include <memory>
#include <cstddef>
namespace cubbl {
class PhysicsWorld {
public:
 PhysicsWorld();
 ~PhysicsWorld();
 PhysicsWorld(const PhysicsWorld&)=delete;
 PhysicsWorld& operator=(const PhysicsWorld&)=delete;
 void add(HouseId id,HousePose pose);
 void remove(HouseId id);
 void step(float dt);
 void applyHorizontalAcceleration(HouseId id,float acceleration);
 HousePose pose(HouseId id) const;
 bool supported(HouseId id) const;
 bool onGround(HouseId id) const;
 bool onTarget(HouseId id,HouseId target) const;
 size_t size() const;
private:
 struct Impl;
 std::unique_ptr<Impl> impl_;
};
}
