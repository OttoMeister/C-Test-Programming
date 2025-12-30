#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BALL_RADIUS 10
#define MAX_SPEED 2.0
#define NUM_BALLS 1000

// Structure to represent a ball
typedef struct {
    double x, y, vx, vy;
    SDL_Color color;
} Ball;

// Function to initialize the positions, velocities, and colors of balls
void initializeBalls(Ball balls[], int numBalls) {
    for (int i = 0; i < numBalls; i++) {
        balls[i] = (Ball){
            .x = rand() % SCREEN_WIDTH,
            .y = rand() % SCREEN_HEIGHT,
            .vx = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
            .vy = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
            .color = {rand() % 256, rand() % 256, rand() % 256}
        };
    }
}

// Function to draw a filled circle on the SDL renderer
void drawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w, dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// Function to apply flocking rules to the balls
void flockingRules(Ball balls[], int numBalls) {
    // Flocking rule parameters
    double separation_radius = 20.0, alignment_radius = 40.0, cohesion_radius = 60.0;
    double max_force = 0.2, max_speed = MAX_SPEED;

    for (int i = 0; i < numBalls; i++) {
        // Initialize rule variables
        double separation_x = 0.0, separation_y = 0.0, alignment_x = 0.0, alignment_y = 0.0;
        double cohesion_x = 0.0, cohesion_y = 0.0;
        int separation_count = 0, alignment_count = 0, cohesion_count = 0;

        for (int j = 0; j < numBalls; j++) {
            if (i != j) {
                double dx = balls[j].x - balls[i].x, dy = balls[j].y - balls[i].y;
                double distance = sqrt(dx * dx + dy * dy);

                // Separation rule
                if (distance < separation_radius) {
                    separation_x -= dx / (distance / 15);
                    separation_y -= dy / (distance / 15);
                    separation_count++;
                }

                // Alignment rule
                if (distance < alignment_radius) {
                    alignment_x += balls[j].vx;
                    alignment_y += balls[j].vy;
                    alignment_count++;
                }

                // Cohesion rule
                if (distance < cohesion_radius) {
                    cohesion_x += balls[j].x;
                    cohesion_y += balls[j].y;
                    cohesion_count++;
                }
            }
        }

        // Calculate averages for alignment and cohesion
        if (separation_count > 0) {
            separation_x /= separation_count;
            separation_y /= separation_count;
        }

        if (alignment_count > 0) {
            alignment_x /= alignment_count;
            alignment_y /= alignment_count;
        }

        if (cohesion_count > 0) {
            cohesion_x /= cohesion_count;
            cohesion_y /= cohesion_count;
            cohesion_x = (cohesion_x - balls[i].x) / 100.0;
            cohesion_y = (cohesion_y - balls[i].y) / 100.0;
        }

        // Calculate total force and limit its magnitude
        double total_force_x = separation_x + alignment_x + cohesion_x;
        double total_force_y = separation_y + alignment_y + cohesion_y;
        double force_magnitude = sqrt(total_force_x * total_force_x + total_force_y * total_force_y);

        if (force_magnitude > max_force) {
            total_force_x = (total_force_x / force_magnitude) * max_force;
            total_force_y = (total_force_y / force_magnitude) * max_force;
        }

        // Update ball velocity
        balls[i].vx += total_force_x;
        balls[i].vy += total_force_y;

        // Limit the speed of the ball
        double speed = sqrt(balls[i].vx * balls[i].vx + balls[i].vy * balls[i].vy);

        if (speed > max_speed) {
            balls[i].vx = (balls[i].vx / speed) * max_speed;
            balls[i].vy = (balls[i].vy / speed) * max_speed;
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool quit = false;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    window = SDL_CreateWindow("Flocking Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Initialize an array of balls
    Ball balls[NUM_BALLS];
    initializeBalls(balls, NUM_BALLS);

    // Main loop
    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Apply flocking rules to the balls
        flockingRules(balls, NUM_BALLS);

        // Update and draw the balls, handling bouncing at borders
        for (int i = 0; i < NUM_BALLS; i++) {
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;

            // Bounce at the borders
            if (balls[i].x < BALL_RADIUS || balls[i].x > SCREEN_WIDTH - BALL_RADIUS) {
                balls[i].vx = -balls[i].vx;
            }

            if (balls[i].y < BALL_RADIUS || balls[i].y > SCREEN_HEIGHT - BALL_RADIUS) {
                balls[i].vy = -balls[i].vy;
            }

            // Draw the ball
            drawFilledCircle(renderer, balls[i].x, balls[i].y, BALL_RADIUS, balls[i].color);
        }

        // Render the scene
        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Add a small delay to control the frame rate
    }

    // Cleanup and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

