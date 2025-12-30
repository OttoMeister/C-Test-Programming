// Dear ImGui: standalone example application for SDL2 + OpenGL3
// git clone https://github.com/ocornut/imgui

// cd ~/CodePlay && time g++ -o test src/sdl3gui_04.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdlrenderer2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test
// https://formatter.org/ Coding Style Google - Column Limit 160

#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

bool InitializeSDL(SDL_Window*& window, SDL_GLContext& glContext) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    return false;
  }
  SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  window = SDL_CreateWindow("ImGui Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, windowFlags);
  if (!window) {
    fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }
  glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    fprintf(stderr, "GL context creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }
  SDL_GL_MakeCurrent(window, glContext);
  SDL_GL_SetSwapInterval(1);  // Enable vsync
  return true;
}

void Cleanup(SDL_Window* window, SDL_GLContext glContext) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main() {
  SDL_Window* window = nullptr;
  SDL_GLContext glContext;
  if (!InitializeSDL(window, glContext)) {
    return -1;
  }
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForOpenGL(window, glContext);
  ImGui_ImplOpenGL3_Init("#version 130");
  ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool done = false;
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT 
      || (event.type == SDL_WINDOWEVENT 
      && event.window.event == SDL_WINDOWEVENT_CLOSE 
      && event.window.windowID == SDL_GetWindowID(window))) {
      done = true;
      }
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    {
      static float sliderValue = 0.0f;
      static int buttonCounter = 0;
      ImGui::Begin("Hello, world!");
      ImGui::Text("This is some useful text.");
      ImGui::SliderFloat("float", &sliderValue, 0.0f, 1.0f);
      if (ImGui::Button("Button")) {
        buttonCounter++;
      }
      ImGui::SameLine();
      ImGui::Text("counter = %d", buttonCounter);
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }
    ImGui::Render();
   
    int displayW, displayH;
    SDL_GL_GetDrawableSize(window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  Cleanup(window, glContext);
  return 0;
}

