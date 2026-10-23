#include "Renderer.h"
#include "native-runtime/Camera.h"
#include "procedural-scene/City.h"
#include "procedural-scene/Platform.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>
#include <algorithm>
#include <cmath>
namespace {
GLuint compile(GLenum type,const char* source) {
 GLuint shader=glCreateShader(type); glShaderSource(shader,1,&source,nullptr); glCompileShader(shader);
 GLint ok=0; glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
 if(!ok){char message[1024];glGetShaderInfoLog(shader,sizeof(message),nullptr,message);__android_log_print(ANDROID_LOG_ERROR,"Cubbl","Shader: %s",message);glDeleteShader(shader);return 0;}return shader;
}
const char* vertex=R"(#version 300 es
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 center;
layout(location=3) in vec3 scale;
layout(location=4) in vec3 color;
layout(location=5) in float angle;
uniform vec4 camera;
uniform vec3 worldTint;
uniform vec3 lightDirection;
uniform vec3 keyColor;
out vec3 tint;
out vec3 baseColor;
out highp vec3 scenePosition;
flat out float cityPart;
flat out float luminous;
flat out float upward;
void main(){
 vec3 local=position*scale;
 mat2 rotation=mat2(cos(angle),sin(angle),-sin(angle),cos(angle));
 vec3 p=vec3(rotation*local.xy,local.z)+center;
 scenePosition=p;
 baseColor=color;
 cityPart=center.z < -3. ? 1. : 0.;
 luminous=step(.9,color.r)*step(.65,color.g)*(1.-step(.65,color.b));
 vec3 worldNormal=vec3(rotation*normal.xy,normal.z);
 upward=step(.5,worldNormal.y);
 p.x-=camera.x;
 float rx=.9659258*p.x-.258819*p.z;
 float rz=.258819*p.x+.9659258*p.z;
 float dy=p.y-camera.y;
 float ry=.9848078*dy-.1736482*rz;
 float depth=.1736482*dy+.9848078*rz;
 gl_Position=vec4(rx/(camera.w*camera.z),ry/camera.w,-depth/100.,1.);
 float facing=max(0.,dot(worldNormal,normalize(lightDirection)));
 float ambient=cityPart>.5?.48:.65;
 float diffuse=cityPart>.5?.52:.35;
 tint=color*worldTint*(vec3(ambient)+diffuse*facing*keyColor);
})";
const char* fragment=R"(#version 300 es
precision mediump float;
in vec3 tint;
in vec3 baseColor;
in highp vec3 scenePosition;
flat in float cityPart;
flat in float luminous;
flat in float upward;
uniform vec3 fogColor;
uniform vec3 fogHorizon;
uniform highp vec2 cityOrigin;
uniform float fogDensity;
uniform float glow;
out vec4 outColor;
void main(){
 vec3 color=tint;
 if(cityPart>.5){
  highp float height=scenePosition.y-cityOrigin.y;
  float lowMist=1.-smoothstep(0.,4.5,height);
  float distanceMist=smoothstep(6.,17.,-scenePosition.z);
  float haze=clamp(.025+fogDensity*distanceMist*mix(.78,1.,lowMist),0.,.78);
  float nearMist=smoothstep(6.8,8.,-scenePosition.z)*(1.-smoothstep(9.,11.,-scenePosition.z));
  haze=1.-(1.-haze)*(1.-nearMist*.62*mix(.55,1.,lowMist));
  if(glow>0. && upward>.5 && height<.35){
   // Street lamps repeat every 2.7 units in the procedural city.
   float dx=mod(scenePosition.x-cityOrigin.x-1.1+1.35,2.7)-1.35;
   float dz=scenePosition.z+7.8;
   float pool=exp(-3.*(dx*dx+.9*dz*dz))*(1.-smoothstep(.05,.35,height));
   color+=vec3(.20,.12,.035)*pool*glow;
  }
  color=mix(color,mix(fogColor,fogHorizon,lowMist*.28),haze);
  color=mix(color,baseColor*1.05,luminous*glow*(1.-.55*haze));
 }
 outColor=vec4(color,1.);
}
)";
const char* skyVertex=R"(#version 300 es
out vec2 uv;
void main(){vec2 p=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));uv=p;gl_Position=vec4(p*2.-1.,0.,1.);}
)";
const char* skyFragment=R"(#version 300 es
precision highp float;
in vec2 uv;
uniform vec3 atmosphere;
uniform vec3 skyPalette[3];
uniform vec3 celestial;
uniform vec3 orbTint;
uniform vec3 cloudTint;
out vec4 outColor;
float puff(vec2 p){
 float d=min(length((p-vec2(-.32,0.))*vec2(1.,1.7))-.38,length((p-vec2(.08,.12))*vec2(1.,1.3))-.40);
 d=min(d,length((p-vec2(.46,-.02))*vec2(1.,1.9))-.30);
 return 1.-smoothstep(-.035,.035,d);
}
float segment(vec2 p,vec2 a,vec2 b){vec2 v=b-a;return length(p-a-v*clamp(dot(p-a,v)/dot(v,v),0.,1.));}
void main(){
 float t=atmosphere.y;
 float night=celestial.z;
 vec3 sky=mix(skyPalette[0],skyPalette[1],smoothstep(.12,.57,uv.y));
 sky=mix(sky,skyPalette[2],smoothstep(.45,1.,uv.y));
 if(night>.5){
  vec2 grid=uv*vec2(atmosphere.x,1.)*36.;
  vec2 cell=floor(grid);
  float seed=fract(sin(dot(cell,vec2(127.1,311.7)))*43758.5453);
  vec2 spot=vec2(.2+.6*seed,.2+.6*fract(seed*19.19));
  float star=(1.-smoothstep(.025,.075,length(fract(grid)-spot)))*step(.82,seed)*smoothstep(.42,.65,uv.y);
  sky=mix(sky,vec3(.85,.92,1.),star*(.65+.25*sin(t*.9+seed*30.)));
 }
 vec2 sun=(uv-celestial.xy)*vec2(atmosphere.x,1.);
 float radius=length(sun);
 sky=mix(sky,cloudTint,.22*(1.-smoothstep(.015,.24,radius)));
 float disk=1.-smoothstep(.044,.048,radius);
 if(night>.5)disk*=smoothstep(.038,.042,length(sun-vec2(.021,.012)));
 sky=mix(sky,orbTint,disk);
 for(int row=0;row<2;++row){
  float scale=row==0?.085:.06;
  float x=mod(uv.x*atmosphere.x+t*(row==0?.008:-.005)+float(row)*.27,.62)-.31;
  vec2 p=vec2(x,uv.y-(row==0?.61:.83))/scale;
  float cloud=puff(p);
  vec3 color=cloudTint*mix(.78,1.,smoothstep(-.3,.3,p.y));
  sky=mix(sky,color,cloud*(row==0?.66:.45)*mix(1.,.5,night));
 }
 for(int i=0;i<3;++i){
  float n=float(i);
  vec2 center=vec2(mod(.18+n*.055+t*.007,1.2)-.1,.72+n*.025+sin(t*.35+n)*.008);
  vec2 p=(uv-center)*vec2(atmosphere.x,1.);
  float flap=sin(t*2.4+n)*.004;
  float bird=min(segment(p,vec2(-.012,.006+flap),vec2(0.,0.)),segment(p,vec2(0.,0.),vec2(.012,.006+flap)));
  sky=mix(sky,vec3(.27,.32,.46),.68*(1.-night)*(1.-smoothstep(.0005,.0018,bird)));
 }
 float grain=fract(sin(dot(gl_FragCoord.xy,vec2(12.9898,78.233)))*43758.5453)-.5;
 outColor=vec4(sky+grain/255.,1.);
}
)";
GLuint createProgram(const char* vertexSource,const char* fragmentSource){
 GLuint vs=compile(GL_VERTEX_SHADER,vertexSource),fs=compile(GL_FRAGMENT_SHADER,fragmentSource);
 if(!vs||!fs){if(vs)glDeleteShader(vs);if(fs)glDeleteShader(fs);return 0;}
 GLuint program=glCreateProgram();glAttachShader(program,vs);glAttachShader(program,fs);glLinkProgram(program);glDeleteShader(vs);glDeleteShader(fs);
 GLint ok=0;glGetProgramiv(program,GL_LINK_STATUS,&ok);
 if(!ok){char log[1024];glGetProgramInfoLog(program,sizeof(log),nullptr,log);__android_log_print(ANDROID_LOG_ERROR,"Cubbl","Program: %s",log);glDeleteProgram(program);return 0;}
 return program;
}

}
Renderer::Renderer(android_app* app,cubbl::CameraFrame& camera,const cubbl::RunAtmosphere& atmosphere):camera_(camera),atmosphere_(atmosphere) {
 display_=eglGetDisplay(EGL_DEFAULT_DISPLAY);
 if(display_==EGL_NO_DISPLAY || !eglInitialize(display_,nullptr,nullptr)) return;
 const EGLint attributes[]={EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,EGL_SURFACE_TYPE,EGL_WINDOW_BIT,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_DEPTH_SIZE,24,EGL_NONE};
 EGLConfig config; EGLint count=0;
 if(!eglChooseConfig(display_,attributes,&config,1,&count)||!count) return;
 EGLint format; eglGetConfigAttrib(display_,config,EGL_NATIVE_VISUAL_ID,&format);
 ANativeWindow_setBuffersGeometry(app->window,0,0,format);
 surface_=eglCreateWindowSurface(display_,config,app->window,nullptr);
 const EGLint ctx[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
 context_=eglCreateContext(display_,config,EGL_NO_CONTEXT,ctx);
 if(surface_==EGL_NO_SURFACE||context_==EGL_NO_CONTEXT||!eglMakeCurrent(display_,surface_,surface_,context_)) return;
 eglSwapInterval(display_,1);
 program_=createProgram(vertex,fragment);skyProgram_=createProgram(skyVertex,skyFragment);
 if(!program_ || !skyProgram_)return;
 skyUniform_=glGetUniformLocation(skyProgram_,"atmosphere");
 skyPalette_=glGetUniformLocation(skyProgram_,"skyPalette[0]");
 celestialUniform_=glGetUniformLocation(skyProgram_,"celestial");
 cloudUniform_=glGetUniformLocation(skyProgram_,"cloudTint");
 orbUniform_=glGetUniformLocation(skyProgram_,"orbTint");
 worldTintUniform_=glGetUniformLocation(program_,"worldTint");
 fogUniform_=glGetUniformLocation(program_,"fogColor");
 glowUniform_=glGetUniformLocation(program_,"glow");
 lightDirectionUniform_=glGetUniformLocation(program_,"lightDirection");
 keyColorUniform_=glGetUniformLocation(program_,"keyColor");
 fogDensityUniform_=glGetUniformLocation(program_,"fogDensity");
 cityOriginUniform_=glGetUniformLocation(program_,"cityOrigin");
 fogHorizonUniform_=glGetUniformLocation(program_,"fogHorizon");
 cameraUniform_=glGetUniformLocation(program_,"camera");
 std::vector<float> vertices;
 const cubbl::Vec3 normals[]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
 const int order[]={0,1,2,0,2,3};
 for(auto n:normals){
  cubbl::Vec3 u=n.x!=0?cubbl::Vec3{0,1,0}:cubbl::Vec3{1,0,0};
  cubbl::Vec3 v={n.y*u.z-n.z*u.y,n.z*u.x-n.x*u.z,n.x*u.y-n.y*u.x};
  const float signs[4][2]={{-1,-1},{1,-1},{1,1},{-1,1}};
  for(int i:order){float a=signs[i][0],b=signs[i][1];vertices.insert(vertices.end(),{(n.x+a*u.x+b*v.x)*.5f,(n.y+a*u.y+b*v.y)*.5f,(n.z+a*u.z+b*v.z)*.5f,n.x,n.y,n.z});}
 }
 glGenVertexArrays(1,&vao_);glBindVertexArray(vao_);glGenBuffers(1,&mesh_);glBindBuffer(GL_ARRAY_BUFFER,mesh_);glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
 for(int i=0;i<2;++i){glEnableVertexAttribArray(i);glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,6*sizeof(float),reinterpret_cast<void*>(i*3*sizeof(float)));}
 glGenBuffers(1,&instances_);glBindBuffer(GL_ARRAY_BUFFER,instances_);
 for(int i=2;i<5;++i){glEnableVertexAttribArray(i);glVertexAttribPointer(i,3,GL_FLOAT,GL_FALSE,10*sizeof(float),reinterpret_cast<void*>((i-2)*3*sizeof(float)));glVertexAttribDivisor(i,1);}
 glEnableVertexAttribArray(5);glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,10*sizeof(float),reinterpret_cast<void*>(9*sizeof(float)));glVertexAttribDivisor(5,1);
 glEnable(GL_DEPTH_TEST);glClearColor(.77f,.88f,.91f,1);
 parts_.reserve(8192);
 platform_=cubbl::generatePlatform();
 for(int i=0;i<15;++i)templates_[i]=cubbl::generateBuilding(i%3,static_cast<uint32_t>(i));
}
Renderer::~Renderer(){
 if(display_!=EGL_NO_DISPLAY){
  if(context_!=EGL_NO_CONTEXT && surface_!=EGL_NO_SURFACE && eglMakeCurrent(display_,surface_,surface_,context_)){
   if(skyProgram_)glDeleteProgram(skyProgram_);if(program_)glDeleteProgram(program_);if(mesh_)glDeleteBuffers(1,&mesh_);if(instances_)glDeleteBuffers(1,&instances_);if(vao_)glDeleteVertexArrays(1,&vao_);
  }
  eglMakeCurrent(display_,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
  if(context_!=EGL_NO_CONTEXT)eglDestroyContext(display_,context_);
  if(surface_!=EGL_NO_SURFACE)eglDestroySurface(display_,surface_);
  eglTerminate(display_);
 }
}
void Renderer::cube(cubbl::Vec3 p,cubbl::Vec3 s,cubbl::Color c,float angle){parts_.insert(parts_.end(),{p.x,p.y,p.z,s.x,s.y,s.z,c.r,c.g,c.b,angle});}
void Renderer::house(const cubbl::House& h){
 const auto& geometry=templates_[h.seed%15];
 const float c=std::cos(h.pose.angleRadians),s=std::sin(h.pose.angleRadians);
 for(auto& p:geometry.parts){
  const float x=p.position.x,y=p.position.y-cubbl::HouseHeight/2;
  cube({h.pose.centerX+c*x-s*y,h.pose.centerY+s*x+c*y,p.position.z},p.scale,p.color,h.pose.angleRadians);
 }
}
bool Renderer::render(const cubbl::GameState& state,float dt){
 if(!ready())return false;
 EGLint width=0,height=0;
 if(!eglQuerySurface(display_,surface_,EGL_WIDTH,&width)||!eglQuerySurface(display_,surface_,EGL_HEIGHT,&height))return false;
 if(width<=0||height<=0)return true;
 float aspect=float(width)/height;
 cubbl::updateCamera(camera_,state,aspect,dt);
 const float halfHeight=camera_.halfHeight,cameraY_=camera_.y;
 const auto timeOfDay=atmosphere_.current();
 const auto palette=cubbl::atmospherePalette(timeOfDay);
 const auto lighting=cubbl::atmosphereLighting(timeOfDay);
 const float night=timeOfDay==cubbl::TimeOfDay::Night?1.f:0.f;
 glViewport(0,0,width,height);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);parts_.clear();
 if(state.phase!=cubbl::GamePhase::Paused && state.phase!=cubbl::GamePhase::GameOver)sceneryTime_+=std::min(.1f,dt);
 glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);glUseProgram(skyProgram_);glBindVertexArray(vao_);
 const GLfloat skyColors[]={palette.horizon.r,palette.horizon.g,palette.horizon.b,palette.middle.r,palette.middle.g,palette.middle.b,palette.zenith.r,palette.zenith.g,palette.zenith.b};
 glUniform3fv(skyPalette_,3,skyColors);
 glUniform3f(celestialUniform_,palette.celestialX,palette.celestialY,night);
 glUniform3f(orbUniform_,palette.celestial.r,palette.celestial.g,palette.celestial.b);
 glUniform3f(cloudUniform_,palette.cloud.r,palette.cloud.g,palette.cloud.b);
 glUniform3f(skyUniform_,aspect,sceneryTime_,cameraY_);glDrawArrays(GL_TRIANGLES,0,3);
 glDepthMask(GL_TRUE);glEnable(GL_DEPTH_TEST);
 const float cityBase=std::max(-.15f,cameraY_-halfHeight*.78f-std::min(std::max(cameraY_,0.f)*.12f,halfHeight*.3f));
 const float cityX=camera_.x*.82f;
 for(const auto& part:cubbl::generateCity(halfHeight*aspect+3,sceneryTime_,timeOfDay).parts)
  cube({part.position.x+cityX,part.position.y+cityBase,part.position.z},part.scale,part.color);
 for(const auto& part:platform_.parts){
  // Include the projected depth of the ground and fence in vertical culling.
  const float projectedY=.9848078f*(part.position.y-cameraY_)-.1736482f*(.258819f*(part.position.x-camera_.x)+.9659258f*part.position.z);
  const float radius=part.scale.y/2+.1736482f*(.258819f*part.scale.x+.9659258f*part.scale.z)/2;
  if(std::abs(projectedY)<=halfHeight+radius)cube(part.position,part.scale,part.color);
 }
 if(state.phase!=cubbl::GamePhase::Menu){
  for(auto& house:state.tower)if(cubbl::houseVisible(house.pose,camera_,aspect))this->house(house);
 }
 if(state.hasActive && state.phase!=cubbl::GamePhase::Menu){
  house(state.active);
  if(!state.activeReleased){
   const float ropeBottom=state.active.pose.centerY+cubbl::HouseHeight/2+.08f;
   const float ropeTop=std::max(ropeBottom+.1f,cameraY_+halfHeight+2.f);
   cube({state.active.pose.centerX,(ropeBottom+ropeTop)/2,0},{.025f,ropeTop-ropeBottom,.025f},{.27f,.33f,.35f});
   cube({state.active.pose.centerX,ropeBottom,0},{.35f,.12f,.16f},{.88f,.60f,.28f});
  }
 }else if(state.phase==cubbl::GamePhase::Menu){
  house({{0,.65f,0},1,1});house({{0,1.95f,0},5,2});house({{0,3.25f,0},8,3});
 }
 glUseProgram(program_);
 glUniform3f(worldTintUniform_,palette.worldTint.r,palette.worldTint.g,palette.worldTint.b);
 glUniform3f(fogUniform_,palette.middle.r,palette.middle.g,palette.middle.b);
 glUniform3f(fogHorizonUniform_,palette.horizon.r,palette.horizon.g,palette.horizon.b);
 glUniform3f(lightDirectionUniform_,lighting.direction.x,lighting.direction.y,lighting.direction.z);
 glUniform3f(keyColorUniform_,lighting.keyColor.r,lighting.keyColor.g,lighting.keyColor.b);
 glUniform1f(fogDensityUniform_,lighting.fogDensity);
 glUniform1f(glowUniform_,lighting.glow);
 glUniform2f(cityOriginUniform_,cityX,cityBase);
 glUniform4f(cameraUniform_,camera_.x,cameraY_,aspect,halfHeight);glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER,instances_);
 glBufferData(GL_ARRAY_BUFFER,parts_.size()*sizeof(float),parts_.data(),GL_STREAM_DRAW);
 glDrawArraysInstanced(GL_TRIANGLES,0,36,static_cast<GLsizei>(parts_.size()/10));
 const GLenum error=glGetError();
 if(error!=GL_NO_ERROR){__android_log_print(ANDROID_LOG_ERROR,"Cubbl","Cube render failed: 0x%x",error);return false;}
 return eglSwapBuffers(display_,surface_)==EGL_TRUE;
}
