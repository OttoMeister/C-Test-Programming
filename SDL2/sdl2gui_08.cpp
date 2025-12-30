//g++ -o test src/sdl2gui_08.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test

#include <SDL.h>
#include <GL/gl.h>
#include <math.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    auto w = SDL_CreateWindow("Ball", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    auto c = SDL_GL_CreateContext(w);
    
    glOrtho(0, 800, 600, 0, -1, 1);
    
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(w, c);
    ImGui_ImplOpenGL3_Init();
    
    float x = 400, y = 300, vx = 2, vy = 1.5, speed = 1.0f;
    float r = 1.0f, g = 0.0f, b = 0.0f;
    
    SDL_Event e;
    bool running = true;
    
    while(running) {
        while(SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if(e.type == SDL_QUIT) running = false;
        }
        
        x += vx * speed; 
        y += vy * speed;
        if(x < 20 || x > 780) vx = -vx;
        if(y < 20 || y > 580) vy = -vy;
        
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(r, g, b);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) 
            glVertex2f(x + 20*cos(i*0.39), y + 20*sin(i*0.39));
        glEnd();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Controls");
        ImGui::SliderFloat("Speed", &speed, 5.0f, 50.0f);

        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(w);
        SDL_Delay(16);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(c);
    SDL_DestroyWindow(w);
    SDL_Quit();
    return 0;
}
