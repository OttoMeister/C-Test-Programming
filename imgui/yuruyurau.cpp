// thx to x.com/yuruyurau/status/2022526453779435912
// apt install libsdl2-dev libimgui-dev libstb-dev
// g++ -o yuruyurau yuruyurau.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_opengl3.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_sdl2.cpp -I/usr/include/imgui -I/usr/include/imgui/backends -I/usr/include/SDL2 -lSDL2 -lGL -limgui -lstb -lm && ./yuruyurau

#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_sdl2.h>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  auto w=SDL_CreateWindow("Yuruyurau",0,0,800,800,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  auto gl_context = SDL_GL_CreateContext(w);
  SDL_GL_SetSwapInterval(1);
  ImGui::CreateContext(); ImGui::GetIO().IniFilename = nullptr;
  ImGui_ImplSDL2_InitForOpenGL(w, gl_context); ImGui_ImplOpenGL3_Init();
  glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  float t=0,sp=.1,sz=2,z=1,prev_sz=-1;
  int n=20000,W=800,H=800;
  glViewport(0,0,W,H);
  while(true) {
    bool resized=false;
    for(SDL_Event e;SDL_PollEvent(&e); ImGui_ImplSDL2_ProcessEvent(&e)) {
      if(e.type == SDL_QUIT) return 0;
      if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        W=e.window.data1; H=e.window.data2; resized=true;
      }
    }
    if(resized) glViewport(0,0,W,H);
    if(sz!=prev_sz) { glPointSize(sz); prev_sz=sz; }
    glClear(GL_COLOR_BUFFER_BIT); glBegin(GL_POINTS);
    float s=z*fmin(W,H)/400., sx=s*2.f/W, sy=s*2.f/H; t+=sp;
    glColor4f(1,1,1,.38);
    for(int i=0;i<n;i++) {
      float y=i/500., k=cos(y*9)*(y<5?sin(t/8+y)*35:11);
      float E=y/8-13, o=sqrt(k*k+E*E)/6, c=o/3-E/5-t/8+(i&3)*8;
      float q=k*y/19+49+k*sin(y)*sin(o*2-E/5-t);
      glVertex2f((q*sin(c)-79*cos(c/3))*sx,-((q+70)*cos(c))*sy);
    }
    glEnd();
    ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplSDL2_NewFrame(); ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(.0);
    ImGui::Begin(" ",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoBackground|ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Speed",&sp,0,.3);
    ImGui::SliderInt("Points",&n,1000,50000);
    ImGui::SliderFloat("Size",&sz,.5,5);
    ImGui::SliderFloat("Zoom",&z,.5,3);
    ImGui::End(); ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(w);
  }
}
