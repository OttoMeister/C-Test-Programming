// thx to x.com/yuruyurau/status/2022526453779435912
// apt install libsdl2-dev libimgui-dev libstb-dev
// g++ -o yuruyurau yuruyurau.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_opengl3.cpp /usr/share/doc/libimgui-dev/examples/backends/imgui_impl_sdl2.cpp -I/usr/include/imgui -I/usr/include/imgui/backends -I/usr/include/SDL2 -lSDL2 -lGL -limgui -lstb -lm && ./yuruyurau

#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_sdl2.h>

float mag(float x, float y) {
    return sqrt(x * x + y * y);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Generative Art - ImGui", 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        1000, 700, 
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 120");
    
    int num_points = 20000;
    float time = 0.0f;
    float time_speed = M_PI / 30.0f;
    float point_size = 2.0f;
    float zoom = 1.0f;
    bool animate = true;
    
    int window_width = 1000, window_height = 700;
    
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
        
        if (animate) {
            time += time_speed;
        }
        
        glClearColor(0.024f, 0.024f, 0.024f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Nutze das gesamte Fenster mit dynamischer Skalierung
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        // Zentriere die Animation und skaliere auf Fenstergröße
        float scale = zoom * fmin(window_width, window_height) / 400.0f;
        float offset_x = window_width / 2.0f;
        float offset_y = window_height / 2.0f;
        
        glOrtho(0, window_width, window_height, 0, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glPointSize(point_size);
        glBegin(GL_POINTS);
        
        for (int i = 0; i < num_points; i++) {
            float y = i / 500.0f;
            float t = time;
            
            float k = cos(y * 9) * (y < 5 ? sin(t / 8 + y) * 35 : 11);
            float e = y / 8 - 13;
            float o = mag(k, e) / 6;
            float q = k * y / 19 + 49 + k * sin(y) * sin(o * 2 - e / 5 - t);
            float c = o / 3 - e / 5 - t / 8 + (i % 4) * 8;
            
            // Original-Koordinaten um (200, 200) zentriert
            float px = q * sin(c) - 79 * cos(c / 3) + 200;
            float py = 200 + (q + 70) * cos(c);
            
            // Transformiere auf Fenstergröße
            float screen_x = (px - 200) * scale + offset_x;
            float screen_y = (py - 200) * scale + offset_y;
            
            glColor4f(1.0f, 1.0f, 1.0f, 0.376f);
            glVertex2f(screen_x, screen_y);
        }
        
        glEnd();
        glDisable(GL_BLEND);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Generative Art Controls");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        
        ImGui::Checkbox("Animate", &animate);
        ImGui::SliderFloat("Speed", &time_speed, 0.001f, 0.3f);
        if (ImGui::Button("Reset Time")) time = 0.0f;
        
        ImGui::Separator();
        ImGui::SliderInt("Points", &num_points, 1000, 50000);
        ImGui::SliderFloat("Size", &point_size, 0.5f, 5.0f);
        ImGui::SliderFloat("Zoom", &zoom, 0.5f, 3.0f);
        
        ImGui::Separator();
        ImGui::Text("Time: %.2f", time);
        ImGui::Text("Window: %dx%d", window_width, window_height);
        
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
