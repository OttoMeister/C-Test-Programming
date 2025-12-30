#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>
#include <math.h>
#include <vector>
#include <random>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

struct Fish {
    float x, y;
    float vx, vy;
    float angle;
    float size;
    float speed;
    float wiggle_phase;
    float wiggle_speed;
    float target_x, target_y;
    
    Fish(float startX, float startY) {
        x = startX;
        y = startY;
        angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        speed = 50.0f + ((float)rand() / RAND_MAX) * 100.0f;
        vx = cos(angle) * speed;
        vy = sin(angle) * speed;
        size = 0.5f + ((float)rand() / RAND_MAX) * 1.0f;
        wiggle_phase = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        wiggle_speed = 2.0f + ((float)rand() / RAND_MAX) * 3.0f;
        target_x = startX;
        target_y = startY;
    }
};

class FishTank {
private:
    std::vector<Fish> fish;
    std::mt19937 rng;
    GLuint backgroundTexture;
    GLuint fishTexture;
    bool texturesLoaded;
    
    GLuint loadTexture(const char* filename) {
        SDL_Surface* surface = IMG_Load(filename);
        if (!surface) {
            std::cout << "Failed to load image: " << filename << " - " << IMG_GetError() << std::endl;
            return 0;
        }
        
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        int Mode = GL_RGB;
        if(surface->format->BytesPerPixel == 4) {
            Mode = GL_RGBA;
        }
        
        glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        SDL_FreeSurface(surface);
        return textureID;
    }
    
public:
    // Simulation parameters
    int fish_count = 30;
    float fish_speed = 1.0f;
    float separation_distance = 60.0f;
    float alignment_distance = 100.0f;
    float cohesion_distance = 120.0f;
    float separation_strength = 1.5f;
    float alignment_strength = 0.5f;
    float cohesion_strength = 0.3f;
    float wall_avoidance = 2.0f;
    float fish_size = 1.0f;
    float wiggle_intensity = 0.2f;
    float wiggle_speed_mult = 1.0f;
    float random_movement = 0.1f;
    float max_speed = 200.0f;
    
    FishTank() : rng(std::random_device{}()), texturesLoaded(false) {
        initializeFish();
    }
    
    bool loadTextures() {
        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
            std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }
        
        // Try to load background (coral reef image)
        backgroundTexture = loadTexture("aquarium.png");
        if (backgroundTexture == 0) {
            backgroundTexture = loadTexture("background.png");
        }
        
        // Try to load fish image
        fishTexture = loadTexture("fish.png");
        
        texturesLoaded = (backgroundTexture != 0 && fishTexture != 0);
        
        if (!texturesLoaded) {
            std::cout << "Warning: Could not load textures. Make sure background.png/jpg and fish.png/jpg are in the same directory as the executable." << std::endl;
        }
        
        return texturesLoaded;
    }
    
    void initializeFish() {
        fish.clear();
        for (int i = 0; i < fish_count; i++) {
            float x = 100.0f + (rand() % 600);
            float y = 100.0f + (rand() % 400);
            fish.emplace_back(x, y);
        }
    }
    
    void update(float deltaTime, int windowWidth, int windowHeight) {
        if (deltaTime > 0.1f) deltaTime = 0.1f; // Cap delta time to prevent large jumps
        
        for (auto& f : fish) {
            float sep_x = 0, sep_y = 0;
            float align_x = 0, align_y = 0;
            float coh_x = 0, coh_y = 0;
            int sep_count = 0, align_count = 0, coh_count = 0;
            
            // Calculate flocking behaviors
            for (const auto& other : fish) {
                if (&f == &other) continue;
                
                float dx = other.x - f.x;
                float dy = other.y - f.y;
                float dist = sqrt(dx * dx + dy * dy);
                
                if (dist > 0 && dist < separation_distance) {
                    sep_x -= dx / dist;
                    sep_y -= dy / dist;
                    sep_count++;
                }
                
                if (dist > 0 && dist < alignment_distance) {
                    align_x += other.vx;
                    align_y += other.vy;
                    align_count++;
                }
                
                if (dist > 0 && dist < cohesion_distance) {
                    coh_x += other.x;
                    coh_y += other.y;
                    coh_count++;
                }
            }
            
            float force_x = 0, force_y = 0;
            
            // Apply separation
            if (sep_count > 0) {
                force_x += (sep_x / sep_count) * separation_strength;
                force_y += (sep_y / sep_count) * separation_strength;
            }
            
            // Apply alignment
            if (align_count > 0) {
                float avg_vx = align_x / align_count;
                float avg_vy = align_y / align_count;
                force_x += (avg_vx - f.vx) * alignment_strength * 0.01f;
                force_y += (avg_vy - f.vy) * alignment_strength * 0.01f;
            }
            
            // Apply cohesion
            if (coh_count > 0) {
                float center_x = coh_x / coh_count;
                float center_y = coh_y / coh_count;
                force_x += (center_x - f.x) * cohesion_strength * 0.01f;
                force_y += (center_y - f.y) * cohesion_strength * 0.01f;
            }
            
            // Wall avoidance
            float margin = 80.0f;
            if (f.x < margin) force_x += wall_avoidance * (margin - f.x) / margin;
            if (f.x > windowWidth - margin) force_x -= wall_avoidance * (margin - (windowWidth - f.x)) / margin;
            if (f.y < margin) force_y += wall_avoidance * (margin - f.y) / margin;
            if (f.y > windowHeight - margin) force_y -= wall_avoidance * (margin - (windowHeight - f.y)) / margin;
            
            // Add some random movement
            force_x += ((float)rand() / RAND_MAX - 0.5f) * random_movement;
            force_y += ((float)rand() / RAND_MAX - 0.5f) * random_movement;
            
            // Update velocity
            f.vx += force_x * deltaTime;
            f.vy += force_y * deltaTime;
            
            // Limit speed
            float speed = sqrt(f.vx * f.vx + f.vy * f.vy);
            float target_speed = max_speed * fish_speed;
            if (speed > target_speed) {
                f.vx = (f.vx / speed) * target_speed;
                f.vy = (f.vy / speed) * target_speed;
            }
            
            // Ensure minimum speed
            if (speed < target_speed * 0.3f) {
                f.vx += ((float)rand() / RAND_MAX - 0.5f) * 20.0f;
                f.vy += ((float)rand() / RAND_MAX - 0.5f) * 20.0f;
            }
            
            // Update position
            f.x += f.vx * deltaTime;
            f.y += f.vy * deltaTime;
            
            // Update angle for fish orientation
            if (speed > 1.0f) {
                f.angle = atan2(f.vy, f.vx);
            }
            
            // Update wiggle animation
            f.wiggle_phase += f.wiggle_speed * wiggle_speed_mult * deltaTime;
            if (f.wiggle_phase > 2.0f * M_PI) {
                f.wiggle_phase -= 2.0f * M_PI;
            }
            
            // Wrap around screen if fish goes too far out
            if (f.x < -50) f.x = windowWidth + 50;
            if (f.x > windowWidth + 50) f.x = -50;
            if (f.y < -50) f.y = windowHeight + 50;
            if (f.y > windowHeight + 50) f.y = -50;
        }
    }
    
    void drawTexturedQuad(GLuint texture, float x, float y, float width, float height, float angle = 0) {
        if (texture == 0) return;
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glPushMatrix();
        glTranslatef(x + width/2, y + height/2, 0);
        if (angle != 0) {
            glRotatef(angle * 180.0f / M_PI, 0, 0, 1);
        }
        glTranslatef(-width/2, -height/2, 0);
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(width, 0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(width, height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0, height);
        glEnd();
        
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
    
    void drawSimpleFish(const Fish& f) {
        // Fallback drawing if no texture
        glPushMatrix();
        glTranslatef(f.x, f.y, 0);
        glRotatef(f.angle * 180.0f / M_PI, 0, 0, 1);
        glScalef(f.size * fish_size * 20, f.size * fish_size * 20, 1);
        
        // Yellow fish body
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= 20; i++) {
            float angle = (i / 20.0f) * 2.0f * M_PI;
            float tail_wiggle = sin(f.wiggle_phase) * wiggle_intensity * 0.2f;
            float x_offset = (i > 15 || i < 5) ? tail_wiggle : 0;
            glVertex2f(cos(angle) * 1.0f + x_offset, sin(angle) * 0.6f);
        }
        glEnd();
        
        // Fish tail
        float tail_angle = sin(f.wiggle_phase) * wiggle_intensity;
        glBegin(GL_TRIANGLES);
        glVertex2f(-1.0f, 0);
        glVertex2f(-1.8f + cos(tail_angle) * 0.3f, -0.5f + sin(tail_angle) * 0.5f);
        glVertex2f(-1.8f + cos(tail_angle) * 0.3f, 0.5f + sin(tail_angle) * 0.5f);
        glEnd();
        
        // Eye
        glColor3f(0, 0, 0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.5f, 0.2f);
        for (int i = 0; i <= 8; i++) {
            float angle = (i / 8.0f) * 2.0f * M_PI;
            glVertex2f(0.5f + cos(angle) * 0.1f, 0.2f + sin(angle) * 0.1f);
        }
        glEnd();
        
        glPopMatrix();
    }
    
    void render(int windowWidth, int windowHeight) {
        glClearColor(0.0f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw background if loaded
        if (texturesLoaded && backgroundTexture != 0) {
            drawTexturedQuad(backgroundTexture, 0, 0, windowWidth, windowHeight);
        }
        
        // Draw fish
        for (const auto& f : fish) {
            if (texturesLoaded && fishTexture != 0) {
                float fishWidth = 60.0f * f.size * fish_size;
                float fishHeight = 30.0f * f.size * fish_size;
                
                // Add wiggle effect to the angle
                float wiggle_angle = f.angle + sin(f.wiggle_phase) * wiggle_intensity * 0.1f;
                drawTexturedQuad(fishTexture, f.x - fishWidth/2, f.y - fishHeight/2, 
                               fishWidth, fishHeight, wiggle_angle);
            } else {
                drawSimpleFish(f);
            }
        }
    }
    
    void renderUI() {
        ImGui::Begin("Fish Tank Controls");
        
        if (!texturesLoaded) {
            ImGui::TextColored(ImVec4(1,1,0,1), "Warning: Textures not loaded!");
            ImGui::Text("Place 'aquarium.png' and 'fish.png' in executable directory");
            ImGui::Separator();
        }
        
        if (ImGui::Button("Reload Textures")) {
            loadTextures();
        }
        
        if (ImGui::CollapsingHeader("Fish Population", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderInt("Fish Count", &fish_count, 1, 100)) {
                if (fish_count != fish.size()) {
                    initializeFish();
                }
            }
            ImGui::SliderFloat("Fish Size", &fish_size, 0.1f, 3.0f);
        }
        
        if (ImGui::CollapsingHeader("Movement Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Overall Speed", &fish_speed, 0.1f, 5.0f);
            ImGui::SliderFloat("Max Speed", &max_speed, 50.0f, 500.0f);
            ImGui::SliderFloat("Random Movement", &random_movement, 0.0f, 2.0f);
            ImGui::SliderFloat("Wiggle Intensity", &wiggle_intensity, 0.0f, 1.0f);
            ImGui::SliderFloat("Wiggle Speed", &wiggle_speed_mult, 0.1f, 3.0f);
        }
        
        if (ImGui::CollapsingHeader("Flocking Behavior")) {
            ImGui::SliderFloat("Separation Distance", &separation_distance, 20.0f, 150.0f);
            ImGui::SliderFloat("Alignment Distance", &alignment_distance, 50.0f, 200.0f);
            ImGui::SliderFloat("Cohesion Distance", &cohesion_distance, 80.0f, 300.0f);
            ImGui::SliderFloat("Separation Strength", &separation_strength, 0.1f, 5.0f);
            ImGui::SliderFloat("Alignment Strength", &alignment_strength, 0.1f, 2.0f);
            ImGui::SliderFloat("Cohesion Strength", &cohesion_strength, 0.1f, 1.0f);
            ImGui::SliderFloat("Wall Avoidance", &wall_avoidance, 0.1f, 10.0f);
        }
        
        if (ImGui::Button("Reset Fish Positions")) {
            initializeFish();
        }
        
        ImGui::Text("Fish Count: %d", (int)fish.size());
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    ~FishTank() {
        if (backgroundTexture != 0) glDeleteTextures(1, &backgroundTexture);
        if (fishTexture != 0) glDeleteTextures(1, &fishTexture);
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 768;
    
    SDL_Window* window = SDL_CreateWindow(
        "Fish Tank Simulation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Setup OpenGL
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 120");
    
    FishTank tank;
    tank.loadTextures();
    
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int w, h;
                SDL_GetWindowSize(window, &w, &h);
                glViewport(0, 0, w, h);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, w, h, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
            }
        }
        
        // Calculate delta time
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Get current window size
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        
        // Update simulation
        tank.update(deltaTime, windowWidth, windowHeight);
        
        // Render
        tank.render(windowWidth, windowHeight);
        
        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        tank.renderUI();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
