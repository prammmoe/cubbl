#pragma once
#include "HousePose.h"
#include "PhysicsWorld.h"
#include <vector>
namespace cubbl {
constexpr float Pi=3.14159265359f;
enum class PlacementGrade { Perfect, Good, Bad, Miss };
enum class GameAction { Start, Release, Pause, Resume, Restart, ReturnToMenu };
enum class GamePhase { Menu, Swinging, Falling, Landing, MissFalling, Paused, GameOver, Collapsing };
enum class GameOverReason { None, OutOfLives, Collapse };
struct House { HousePose pose; uint32_t seed=0; HouseId id=0; };
struct Stability {
 float lastSupportedY=0, tiltTime=0, fallTime=0;
 bool update(HousePose pose,bool supported,float dt);
};
struct GameState {
 GamePhase phase=GamePhase::Menu;
 int64_t score=0;
 int combo=0,lives=3,successfulPlacements=0;
 float speedMultiplier=1;
 House active;
 bool hasActive=false, activeReleased=false;
 std::vector<House> tower;
 GameOverReason reason=GameOverReason::None;
};
enum class EventType { Landed, Missed, GameOver, Collapsed };
struct GameEvent { EventType type; PlacementGrade grade; int64_t score; };
PlacementGrade classifyPlacement(float offset,float width,float angleDifference=0);
float placementSpeed(int successfulPlacements);
float towerImbalance(const std::vector<House>& tower);
float towerSway(int floors,float imbalance,float seconds,float heightFraction);
struct PlacementReward { int64_t points; int combo; };
PlacementReward placementReward(PlacementGrade grade,int previousCombo);
class Game {
public:
 void dispatch(GameAction action);
 void update(float fixedDeltaSeconds);
 const GameState& state()const{return state_;}
 std::vector<GameEvent> takeEvents();
private:
 void reset();
 void spawn();
 void collapse();
 void end(GameOverReason reason);
 GameState state_;
 GamePhase resumePhase_=GamePhase::Swinging;
 std::unique_ptr<PhysicsWorld> physics_;
 std::vector<Stability> stability_;
 float swayTime_=0,swingTime_=0,phaseTime_=0,landingTime_=0,spawnX_=0,spawnTop_=0;
 HouseId target_=0,nextId_=1;
 std::vector<GameEvent> events_;
};
}
