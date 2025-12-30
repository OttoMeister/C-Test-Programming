// g++ -o test src/sdl2gui_07.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_demo.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test



#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_RADIUS 10
#define MAX_BALL_SPEED 5
#define MAX_NUM_BALLS 200

typedef struct {
    float x;
    float y;
    float dx;
    float dy;
    float r, g, b;
} Ball;

Ball balls[MAX_NUM_BALLS];
int currentNumBalls = 50;

void initializeBalls() {
    for (int i = 0; i < currentNumBalls; i++) {
        balls[i].x = (float)(rand() % (SCREEN_WIDTH - BALL_RADIUS * 2)) + BALL_RADIUS;
        balls[i].y = (float)(rand() % (SCREEN_HEIGHT - BALL_RADIUS * 2)) + BALL_RADIUS;
        balls[i].dx = ((float)(rand() % (2 * MAX_BALL_SPEED * 100)) / 100.0f) - MAX_BALL_SPEED;
        balls[i].dy = ((float)(rand() % (2 * MAX_BALL_SPEED * 100)) / 100.0f) - MAX_BALL_SPEED;
        balls[i].r = (float)(rand() % 256) / 255.0f;
        balls[i].g = (float)(rand() % 256) / 255.0f;
        balls[i].b = (float)(rand() % 256) / 255.0f;
    }
}

void addBall() {
    if (currentNumBalls < MAX_NUM_BALLS) {
        int i = currentNumBalls;
        balls[i].x = (float)(rand() % (SCREEN_WIDTH - BALL_RADIUS * 2)) + BALL_RADIUS;
        balls[i].y = (float)(rand() % (SCREEN_HEIGHT - BALL_RADIUS * 2)) + BALL_RADIUS;
        balls[i].dx = ((float)(rand() % (2 * MAX_BALL_SPEED * 100)) / 100.0f) - MAX_BALL_SPEED;
        balls[i].dy = ((float)(rand() % (2 * MAX_BALL_SPEED * 100)) / 100.0f) - MAX_BALL_SPEED;
        balls[i].r = (float)(rand() % 256) / 255.0f;
        balls[i].g = (float)(rand() % 256) / 255.0f;
        balls[i].b = (float)(rand() % 256) / 255.0f;
        currentNumBalls++;
    }
}

void drawFilledCircleGL(float x, float y, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center point
    
    int segments = 32;
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

bool checkCollision(float x1, float y1, float x2, float y2, float radius1, float radius2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distanceSquared = dx * dx + dy * dy;
    float radiiSumSquared = (radius1 + radius2) * (radius1 + radius2);
    return distanceSquared <= radiiSumSquared;
}

void updateBalls(float speedMultiplier) {
    for (int i = 0; i < currentNumBalls; i++) {
        balls[i].x += balls[i].dx * speedMultiplier;
        balls[i].y += balls[i].dy * speedMultiplier;

        // Bounce off screen edges
        if (balls[i].x < BALL_RADIUS || balls[i].x > SCREEN_WIDTH - BALL_RADIUS) {
            balls[i].dx = -balls[i].dx;
            balls[i].x = fmax(BALL_RADIUS, fmin(SCREEN_WIDTH - BALL_RADIUS, balls[i].x));
        }
        if (balls[i].y < BALL_RADIUS || balls[i].y > SCREEN_HEIGHT - BALL_RADIUS) {
            balls[i].dy = -balls[i].dy;
            balls[i].y = fmax(BALL_RADIUS, fmin(SCREEN_HEIGHT - BALL_RADIUS, balls[i].y));
        }

        // Check collisions with other balls
        for (int j = i + 1; j < currentNumBalls; j++) {
            if (checkCollision(balls[i].x, balls[i].y, balls[j].x, balls[j].y, BALL_RADIUS, BALL_RADIUS)) {
                // Simple collision response - swap velocities
                float temp_dx = balls[i].dx;
                float temp_dy = balls[i].dy;
                balls[i].dx = balls[j].dx;
                balls[i].dy = balls[j].dy;
                balls[j].dx = temp_dx;
                balls[j].dy = temp_dy;

                // Separate balls to prevent overlap
                float dx = balls[j].x - balls[i].x;
                float dy = balls[j].y - balls[i].y;
                float distance = sqrtf(dx * dx + dy * dy);
                if (distance > 0) {
                    float overlap = 2 * BALL_RADIUS - distance;
                    float separateX = (dx / distance) * (overlap / 2);
                    float separateY = (dy / distance) * (overlap / 2);
                    balls[i].x -= separateX;
                    balls[i].y -= separateY;
                    balls[j].x += separateX;
                    balls[j].y += separateY;
                }
            }
        }
    }
}

void renderBalls() {
    // Set up 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Enable blending for smooth circles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw all balls
    for (int i = 0; i < currentNumBalls; i++) {
        drawFilledCircleGL(balls[i].x, balls[i].y, BALL_RADIUS, balls[i].r, balls[i].g, balls[i].b);
    }

    glDisable(GL_BLEND);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
        "SDL2 + Dear ImGui - Ball Control Demo", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    srand((unsigned int)time(NULL));
    initializeBalls();

    // Main loop variables
    bool quit = false;
    SDL_Event event;
    
    // ImGui state variables
    static float ballSpeed = 1.0f;
    static int targetNumBalls = currentNumBalls;
    static bool isPaused = false;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }

        // Update ball count if changed
        if (targetNumBalls != currentNumBalls) {
            if (targetNumBalls > currentNumBalls) {
                // Add balls
                while (currentNumBalls < targetNumBalls && currentNumBalls < MAX_NUM_BALLS) {
                    addBall();
                }
            } else {
                // Remove balls (simply reduce the count)
                currentNumBalls = targetNumBalls;
                if (currentNumBalls < 0) currentNumBalls = 0;
            }
        }

        // Update simulation
        if (!isPaused) {
            updateBalls(ballSpeed);
        }

        // Get window size for proper viewport
        int display_w, display_h;
        SDL_GetWindowSize(window, &display_w, &display_h);

        // Clear screen
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render balls using OpenGL
        renderBalls();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Create ImGui control panel
        ImGui::Begin("Ball Control Panel");
        
        // Speed control
        ImGui::SliderFloat("Ball Speed", &ballSpeed, 0.0f, 3.0f, "%.2f");
        
        // Ball count control
        ImGui::SliderInt("Number of Balls", &targetNumBalls, 1, MAX_NUM_BALLS);
  
        ImGui::Separator();
        
        // Display info
        ImGui::Text("Performance:");
        ImGui::SameLine();
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);

        
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
        SDL_Delay(16); // ~60 FPS
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
