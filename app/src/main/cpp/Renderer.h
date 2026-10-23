#pragma once
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <vector>
#include "game-core/Game.h"
#include "native-runtime/Camera.h"
#include "procedural-scene/Building.h"
#include "procedural-scene/Atmosphere.h"
struct android_app;
class Renderer {
public:
 explicit Renderer(android_app* app,cubbl::CameraFrame& camera,const cubbl::RunAtmosphere& atmosphere);
 ~Renderer();
 bool ready() const { return program_!=0 && skyProgram_!=0; }
 bool render(const cubbl::GameState& state,float dt);
private:
 void cube(cubbl::Vec3 p,cubbl::Vec3 scale,cubbl::Color color,float angle=0);
 void house(const cubbl::House& h);
 EGLDisplay display_=EGL_NO_DISPLAY;
 EGLSurface surface_=EGL_NO_SURFACE;
 EGLContext context_=EGL_NO_CONTEXT;
 GLuint skyProgram_=0;
 GLint skyUniform_=-1,skyPalette_=-1,celestialUniform_=-1,cloudUniform_=-1,orbUniform_=-1;
 float sceneryTime_=0;
 GLuint program_=0, vao_=0, mesh_=0, instances_=0;
 GLint cameraUniform_=-1,worldTintUniform_=-1,fogUniform_=-1,glowUniform_=-1;
 GLint lightDirectionUniform_=-1,keyColorUniform_=-1,fogDensityUniform_=-1,cityOriginUniform_=-1,fogHorizonUniform_=-1;
 cubbl::CameraFrame& camera_;
 const cubbl::RunAtmosphere& atmosphere_;
 std::vector<float> parts_;
 cubbl::BuildingGeometry templates_[15],platform_;
};
