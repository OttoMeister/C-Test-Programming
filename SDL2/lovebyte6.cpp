//g++ -o test src/lovebyte6.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test


#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Simple Math Viz", 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        800, 600, 
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 120");
    
    int n = 40, m = 200;
    float r = 0.1f, time_factor = 0.01f, scale = 2.0f, time = 0.0f;
    int window_width = 800, window_height = 600;
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    glViewport(0, 0, window_width, window_height);
                }
            }
        }
        time += time_factor;
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // Maintain aspect ratio
        float aspect = (float)window_width / (float)window_height;
        if (aspect >= 1.0f) {
            glOrtho(-scale * aspect, scale * aspect, -scale, scale, -1, 1);
        } else {
            glOrtho(-scale, scale, -scale / aspect, scale / aspect, -1, 1);
        }
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        float x = 0, v = 0, t = time;
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                float u = sin(i + v) + sin(r * i + x);
                v = cos(i + v) + cos(r * i + x);
                x = u + t;
                glColor4f((float)i / n, (float)j / m, 0.4f, 0.8f);
                glVertex2f(u, v);
            }
        }
        glEnd();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Controls");
        ImGui::SliderInt("n (iterations)", &n, 1, 200);
        ImGui::SliderInt("m (steps)", &m, 1, 500);
        ImGui::SliderFloat("r (frequency)", &r, 0.01f, 1.0f);
        ImGui::SliderFloat("Speed", &time_factor, 0.001f, 0.1f);
        ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
