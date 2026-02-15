// thx to x.com/yuruyurau/status/2022526453779435912
// apt install libsdl2-dev libimgui-dev libstb-dev
// g++ -o yuruyurau yuruyurau.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_opengl3.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_sdl2.cpp -I/usr/include/imgui -I/usr/include/imgui/backends -I/usr/include/SDL2 -lSDL2 -lGL -limgui -lstb -lm && ./yuruyurau

#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_sdl2.h>

int main(){
  SDL_Init(32);
  auto w=SDL_CreateWindow("Yuruyurau",0,0,800,800,34);
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForOpenGL(w,SDL_GL_CreateContext(w));
  ImGui_ImplOpenGL3_Init();
  bool a=1,r=1;
  float t=0,sp=.1f,sz=2,z=1;
  int n=20000,W,H;
  while(r) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      ImGui_ImplSDL2_ProcessEvent(&e);
      if(e.type==256)r=0; }
    SDL_GetWindowSize(w,&W,&H);
    glViewport(0,0,W,H);
    if(a)t+=sp;
    glClear(16384);glEnable(3042);glBlendFunc(770,771);glPointSize(sz);glBegin(0);
    float s=z*fmin(W,H)/400.f;
    for(int i=0;i<n;i++) {
      float y=i/500.f;
      float k=cos(y*9)*(y<5?sin(t/8+y)*35:11);
      float E=y/8-13,o=sqrt(k*k+E*E)/6,c=o/3-E/5-t/8+(i%4)*8;
      float q=k*y/19+49+k*sin(y)*sin(o*2-E/5-t);
      glColor4f(1,1,1,.38f);
      glVertex2f((q*sin(c)-79*cos(c/3))*s/(W/2.f),-((q+70)*cos(c))*s/(H/2.f)); }
    glEnd();
    ImGui_ImplOpenGL3_NewFrame();ImGui_ImplSDL2_NewFrame();ImGui::NewFrame();
    ImGui::Begin("Controls");
    ImGui::Checkbox("Animate",&a);
    ImGui::SliderFloat("Speed",&sp,0,.3f);
    ImGui::SliderInt("Points",&n,1000,50000);
    ImGui::SliderFloat("Size",&sz,.5f,5);
    ImGui::SliderFloat("Zoom",&z,.5f,3);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(w);
}}

