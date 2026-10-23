#include <jni.h>
#include <android/looper.h>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <array>
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <algorithm>
#include "Renderer.h"
#include "native-runtime/FrameClock.h"
namespace {
using namespace cubbl;
Game game;
CameraFrame camera;
RunAtmosphere atmosphere(static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count()));
std::mutex bridgeMutex;
std::deque<GameAction> commands;
// JNI snapshot: phase, score, combo, lives, placements, feedback, event sequence, renderer error, game-over reason.
std::array<jlong,9> snapshot={0,0,0,3,0,-1,0,0,0};
ALooper* gameLooper=nullptr;
int feedback=-1; jlong sequence=0;
struct Runtime {
 std::unique_ptr<Renderer> renderer;
 bool focused=false, resumed=false, dirty=true, error=false;
 int recoveryAttempts=0;
};
void publish(const Runtime& runtime) {
 for(const auto& event:game.takeEvents()) {
  feedback=event.type==EventType::Landed?static_cast<int>(event.grade):(event.type==EventType::Missed?3:(event.type==EventType::Collapsed?5:4)); ++sequence;
 }
 const auto& s=game.state();
 std::lock_guard<std::mutex> lock(bridgeMutex);
 snapshot={static_cast<jlong>(s.phase),s.score,s.combo,s.lives,s.successfulPlacements,feedback,sequence,runtime.error?1:0,static_cast<jlong>(s.reason)};
}
void handleCommand(android_app* app,int32_t command) {
 auto& r=*static_cast<Runtime*>(app->userData); r.dirty=true;
#ifndef NDEBUG
 __android_log_print(ANDROID_LOG_DEBUG,"Cubbl","Lifecycle app=%p command=%d",app,command);
#endif
 switch(command){
 case APP_CMD_INIT_WINDOW:
  if(app->window){r.renderer.reset();r.renderer=std::make_unique<Renderer>(app,camera,atmosphere);r.error=!r.renderer->ready();r.recoveryAttempts=0;}break;
 case APP_CMD_TERM_WINDOW:r.renderer.reset();game.dispatch(GameAction::Pause);break;
 case APP_CMD_GAINED_FOCUS:r.focused=true;break;
 case APP_CMD_RESUME:r.resumed=true;break;
 case APP_CMD_LOST_FOCUS:r.focused=false;game.dispatch(GameAction::Pause);break;
 case APP_CMD_PAUSE:case APP_CMD_STOP:
  r.resumed=false;game.dispatch(GameAction::Pause);break;
 default:break;
 }
}
}
extern "C" JNIEXPORT void JNICALL Java_com_example_cubbl_MainActivity_nativeCommand(JNIEnv*,jobject,jint action){
 if(action<0||action>5)return;
#ifndef NDEBUG
 __android_log_print(ANDROID_LOG_DEBUG,"Cubbl","UI action=%d",action);
#endif
 std::lock_guard<std::mutex> lock(bridgeMutex);
 if(commands.size()<32)commands.push_back(static_cast<GameAction>(action));
 if(gameLooper)ALooper_wake(gameLooper);
}
extern "C" JNIEXPORT jlongArray JNICALL Java_com_example_cubbl_MainActivity_nativeSnapshot(JNIEnv* env,jobject){
 std::array<jlong,9> copy;
 {std::lock_guard<std::mutex> lock(bridgeMutex);copy=snapshot;}
 auto array=env->NewLongArray(copy.size());if(array)env->SetLongArrayRegion(array,0,copy.size(),copy.data());return array;
}
extern "C" void android_main(android_app* app){
 Runtime runtime;app->userData=&runtime;app->onAppCmd=handleCommand;
 // Android Views own input; native glue must not consume Back or queue unused touch events.
 android_app_set_key_event_filter(app,[](const GameActivityKeyEvent*){return false;});
 android_app_set_motion_event_filter(app,[](const GameActivityMotionEvent*){return false;});
 {std::lock_guard<std::mutex> lock(bridgeMutex);gameLooper=ALooper_forThread();ALooper_acquire(gameLooper);}
 using Clock=std::chrono::steady_clock;
 auto last=Clock::now(); FrameClock clock;
 while(!app->destroyRequested){
  auto phase=game.state().phase;
  bool active=runtime.resumed && runtime.focused && runtime.renderer && !runtime.error && phase!=GamePhase::Menu && phase!=GamePhase::Paused && phase!=GamePhase::GameOver;
  bool animateMenu=runtime.resumed && runtime.focused && runtime.renderer && !runtime.error && phase==GamePhase::Menu;
  int timeout=active||animateMenu||runtime.dirty?0:-1;
  int result;
  do {
   int events=0;
   android_poll_source* source=nullptr;
   result=ALooper_pollOnce(timeout,nullptr,&events,reinterpret_cast<void**>(&source));
   if(result>=0 && source)source->process(app,source);
   timeout=0;
  }while(!app->destroyRequested && result!=ALOOPER_POLL_TIMEOUT && result!=ALOOPER_POLL_ERROR);
  if(app->destroyRequested || result==ALOOPER_POLL_ERROR)break;
  std::deque<GameAction> pending;
  {std::lock_guard<std::mutex> lock(bridgeMutex);pending.swap(commands);}
  for(auto action:pending){
   if((!runtime.resumed || !runtime.focused) && (action==GameAction::Resume||action==GameAction::Start||action==GameAction::Restart||action==GameAction::Release)){
    __android_log_print(ANDROID_LOG_WARN,"Cubbl","Ignored action %d: resumed=%d focused=%d",static_cast<int>(action),runtime.resumed,runtime.focused);continue;
   }
#ifndef NDEBUG
   __android_log_print(ANDROID_LOG_DEBUG,"Cubbl","Dispatch action=%d phase=%d",static_cast<int>(action),static_cast<int>(game.state().phase));
#endif
   const bool newRun=action==GameAction::Restart || (action==GameAction::Start && game.state().phase==GamePhase::Menu);
   game.dispatch(action);
   if(newRun){
    atmosphere.nextRun();
#ifndef NDEBUG
    __android_log_print(ANDROID_LOG_DEBUG,"Cubbl","Run atmosphere=%d",static_cast<int>(atmosphere.current()));
#endif
   }
   runtime.dirty=true;clock.reset();
  }
  auto now=Clock::now();if(!pending.empty())last=now;float dt=std::min(.1f,std::chrono::duration<float>(now-last).count());last=now;
  phase=game.state().phase;
  active=runtime.resumed && runtime.focused && runtime.renderer && !runtime.error && phase!=GamePhase::Menu && phase!=GamePhase::Paused && phase!=GamePhase::GameOver;
  for(int count=clock.advance(dt,active);count>0;--count)game.update(1.f/60);
  if(runtime.renderer && runtime.resumed && runtime.focused && !runtime.error && (active||animateMenu||runtime.dirty)){
   if(!runtime.renderer->render(game.state(),std::max(dt,1.f/60))){
    __android_log_print(ANDROID_LOG_ERROR,"Cubbl","EGL swap failed; rebuilding renderer");
    game.dispatch(GameAction::Pause);runtime.renderer.reset();
    if(++runtime.recoveryAttempts<=2 && app->window){runtime.renderer=std::make_unique<Renderer>(app,camera,atmosphere);runtime.error=!runtime.renderer->ready();}
    else runtime.error=true;
   }else runtime.recoveryAttempts=0;
  }
  runtime.dirty=false;publish(runtime);
 }
 game.dispatch(GameAction::Pause);
 {std::lock_guard<std::mutex> lock(bridgeMutex);ALooper_release(gameLooper);gameLooper=nullptr;commands.clear();}
 app->userData=nullptr;
}
