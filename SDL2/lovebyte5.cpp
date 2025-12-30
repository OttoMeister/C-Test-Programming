// g++ -o test src/lovebyte5.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test


#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include <iostream>
#include <vector>
#include <cmath>

class MathVisualizer {
private:
    SDL_Window* window;
    SDL_GLContext gl_context;
    SDL_Renderer* renderer;
    bool running;
    int width, height;
    
    // Visualization parameters
    int n = 40;
    int m = 200;
    float r = 0.1f;
    float time_factor = 0.01f;
    float scale = 4.0f;
    
    // Animation
    float animation_time = 0.0f;
    
public:
    MathVisualizer(int w = 1200, int h = 800) : width(w), height(h), running(true) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            exit(1);
        }
        
        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        
        window = SDL_CreateWindow(
            "Interactive Math Visualization",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
        
        gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        
        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        ImGui::StyleColorsDark();
        
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 120");
        
        // Set up OpenGL
        glViewport(0, 0, width, height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    ~MathVisualizer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        width = event.window.data1;
                        height = event.window.data2;
                        glViewport(0, 0, width, height);
                    }
                    break;
            }
        }
    }
    
    void renderVisualization() {
        // Set up orthographic projection for OpenGL coordinates (-1 to 1)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-scale, scale, -scale, scale, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Your mathematical visualization
        float x = 0, v = 0, t = animation_time * time_factor;
        
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                float u = sin(i + v) + sin(r * i + x);
                v = cos(i + v) + cos(r * i + x);
                x = u + t;
                
                // Set color based on iteration values
                float red = (float)i / n;
                float green = (float)j / m;
                float blue = 0.4f;
                
                glColor4f(red, green, blue, 0.8f);
                glVertex2f(u, v);
            }
        }
        
        glEnd();
    }
    
    void renderImGui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Control Panel
        ImGui::Begin("Visualization Controls");
        
        ImGui::Text("Mathematical Visualization Parameters");
        ImGui::Separator();
        
        ImGui::SliderInt("n (outer iterations)", &n, 1, 100);
        ImGui::SliderInt("m (inner iterations)", &m, 1, 500);
        ImGui::SliderFloat("r (frequency)", &r, 0.01f, 1.0f, "%.3f");
        ImGui::SliderFloat("Time Factor", &time_factor, 0.001f, 0.1f, "%.4f");
        ImGui::SliderFloat("Scale (zoom)", &scale, 1.0f, 10.0f, "%.1f");
     
        ImGui::End();
    }
    
    void run() {
        while (running) {
            handleEvents();
            
            // Update animation
            animation_time += 1.0f;
            
            // Clear screen
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            // Render visualization
            renderVisualization();
            
            // Render ImGui
            renderImGui();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            SDL_GL_SwapWindow(window);
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        MathVisualizer app(1200, 800);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

