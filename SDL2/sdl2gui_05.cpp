// g++ -o test src/sdl2gui_05.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdlrenderer2.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp -Iimgui -Iimgui/backends -I/usr/include/SDL2 -lSDL2 -lGL && timeout 20s ./test

// Flimmern de SDL2-Grafik und das Fehlen von ImGui Fenster


#include <SDL2/SDL.h>
#include <SDL_opengl.h>

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
#define NUM_BALLS 50  // Number of balls to display

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    SDL_Color color;
} Ball;

void initializeBalls(Ball balls[], int numBalls) {
    for (int i = 0; i < numBalls; i++) {
        balls[i].x = rand() % (SCREEN_WIDTH - BALL_RADIUS * 2);
        balls[i].y = rand() % (SCREEN_HEIGHT - BALL_RADIUS * 2);
        balls[i].dx = (rand() % (2 * MAX_BALL_SPEED + 1)) - MAX_BALL_SPEED;
        balls[i].dy = (rand() % (2 * MAX_BALL_SPEED + 1)) - MAX_BALL_SPEED;
        balls[i].color.r = rand() % 256;
        balls[i].color.g = rand() % 256;
        balls[i].color.b = rand() % 256;
    }
}

void drawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

bool checkCollision(int x1, int y1, int x2, int y2, int radius1, int radius2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int distanceSquared = dx * dx + dy * dy;
    int radiiSumSquared = (radius1 + radius2) * (radius1 + radius2);

    return distanceSquared <= radiiSumSquared;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_GLContext gl_context;
    SDL_Event event;
    bool quit = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL2 + Dear ImGui", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    srand(time(NULL));

    Ball balls[NUM_BALLS];
    initializeBalls(balls, NUM_BALLS);

    // Main loop
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // SDL rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < NUM_BALLS; i++) {
            balls[i].x += balls[i].dx;
            balls[i].y += balls[i].dy;

            // Bounce off the screen edges
            if (balls[i].x < 0 || balls[i].x > SCREEN_WIDTH - BALL_RADIUS * 2) {
                balls[i].dx = -balls[i].dx;
            }
            if (balls[i].y < 0 || balls[i].y > SCREEN_HEIGHT - BALL_RADIUS * 2) {
                balls[i].dy = -balls[i].dy;
            }

            // Draw round balls
            drawFilledCircle(renderer, balls[i].x + BALL_RADIUS, balls[i].y + BALL_RADIUS, BALL_RADIUS, balls[i].color);

            // Check for collisions with other balls
            for (int j = 0; j < NUM_BALLS; j++) {
                if (i != j && checkCollision(balls[i].x + BALL_RADIUS, balls[i].y + BALL_RADIUS, balls[j].x + BALL_RADIUS, balls[j].y + BALL_RADIUS, BALL_RADIUS, BALL_RADIUS)) {
                    // Handle the collision by reversing the direction of both balls
                    int temp_dx = balls[i].dx;
                    int temp_dy = balls[i].dy;
                    balls[i].dx = balls[j].dx;
                    balls[i].dy = balls[j].dy;
                    balls[j].dx = temp_dx;
                    balls[j].dy = temp_dy;
                }
            }
        }

        SDL_RenderPresent(renderer);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Show ImGui demo window
        ImGui::Begin("Hello, world!");
        static float sliderValue = 0.0f;
        static int buttonCounter = 0;
        ImGui::Text("This is some useful text.");
        ImGui::SliderFloat("float", &sliderValue, 0.0f, 1.0f);
        if (ImGui::Button("Button")) {
            buttonCounter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", buttonCounter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        SDL_GL_MakeCurrent(window, gl_context);
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        SDL_Delay(10);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
