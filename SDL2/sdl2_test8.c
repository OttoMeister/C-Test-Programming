#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define FLOCK_SIZE 10
#define MAX_SPEED 2.0 
#define NUM_BALLS 500
#define SEPARATION_RADIUS 40.0
#define ALIGNMENT_RADIUS 60.0
#define COHESION_RADIUS 80.0
#define MAX_FORCE 0.2
#define SEPARATION_FORCE 5
#define FRAMERATE_DELAY 5

// Structure to represent a triangle
typedef struct {
    double x, y; // Position of the triangle's center
    double angle; // Angle of the triangle in radians
    double vx, vy; // Velocity components
    SDL_Color color;
} Triangle;

// Function to initialize the positions, velocities, and colors of triangles
void initializeTriangles(Triangle triangles[], int numTriangles, int screenWidth, int screenHeight) {
    for (int i = 0; i < numTriangles; i++) {
        triangles[i] = (Triangle){
            .x = rand() % screenWidth,
            .y = rand() % screenHeight,
            .angle = ((double)rand() / RAND_MAX) * 2 * M_PI, // Random initial angle
            .vx = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
            .vy = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
            .color = {rand() % 256, rand() % 256, rand() % 256}
        };
    }
}

// Function to draw an arrow on the SDL renderer
void drawFlock(SDL_Renderer* renderer, double x, double y, double angle, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    // Define the vertices of the arrow
    double arrowLength = FLOCK_SIZE;
    double arrowWidth = FLOCK_SIZE / 3; // Adjust the width as needed
    double tipX = x + cos(angle) * arrowLength;
    double tipY = y + sin(angle) * arrowLength;
    
    double base1X = x + cos(angle + (5 * M_PI / 6)) * arrowWidth;
    double base1Y = y + sin(angle + (5 * M_PI / 6)) * arrowWidth;
    
    double base2X = x + cos(angle + (7 * M_PI / 6)) * arrowWidth;
    double base2Y = y + sin(angle + (7 * M_PI / 6)) * arrowWidth;

    // Draw the arrow lines
    SDL_RenderDrawLine(renderer, (int)x, (int)y, (int)tipX, (int)tipY);
    SDL_RenderDrawLine(renderer, (int)tipX, (int)tipY, (int)base1X, (int)base1Y);
    SDL_RenderDrawLine(renderer, (int)tipX, (int)tipY, (int)base2X, (int)base2Y);
}






// Function to apply flocking rules to the balls
void flockingRules(Triangle balls[], int numBalls) {
    // Flocking rule parameters
    double separation_radius = SEPARATION_RADIUS;
    double alignment_radius = ALIGNMENT_RADIUS;
    double cohesion_radius = COHESION_RADIUS;
    double max_force = MAX_FORCE;

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
                    separation_x -= dx / (distance / SEPARATION_FORCE);
                    separation_y -= dy / (distance / SEPARATION_FORCE);
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

        // Limit the speed of the ball to MAX_SPEED
        double speed = sqrt(balls[i].vx * balls[i].vx + balls[i].vy * balls[i].vy);

        if (speed > MAX_SPEED) {
            balls[i].vx = (balls[i].vx / speed) * MAX_SPEED;
            balls[i].vy = (balls[i].vy / speed) * MAX_SPEED;
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool quit = false;

    int screenWidth = 800; // Initial screen width
    int screenHeight = 600; // Initial screen height

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a resizable window
    window = SDL_CreateWindow("Moving Triangles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // Create a renderer with antialiasing
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // Enable antialiasing

    if (renderer == NULL) {
        // Handle renderer creation error
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Initialize an array of triangles
    Triangle triangles[NUM_BALLS];
    initializeTriangles(triangles, NUM_BALLS, screenWidth, screenHeight);





    // Main loop
    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_WINDOWEVENT) {
                // Handle window resizing
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    screenWidth = event.window.data1;
                    screenHeight = event.window.data2;
                    initializeTriangles(triangles, NUM_BALLS, screenWidth, screenHeight);
                }
            }
        }

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
               // Apply flocking rules to the balls
        flockingRules(triangles, NUM_BALLS); 

        // Update and draw the triangles
        for (int i = 0; i < NUM_BALLS; i++) {
            // Update triangle position based on velocity
            triangles[i].x += triangles[i].vx;
            triangles[i].y += triangles[i].vy;

            // Bounce at the borders
            if (triangles[i].x < FLOCK_SIZE || triangles[i].x > screenWidth - FLOCK_SIZE) {
                triangles[i].vx = -triangles[i].vx;
            }

            if (triangles[i].y < FLOCK_SIZE || triangles[i].y > screenHeight - FLOCK_SIZE) {
                triangles[i].vy = -triangles[i].vy;
            }

            // Draw the Flock
            drawFlock(renderer, triangles[i].x, triangles[i].y, triangles[i].angle, triangles[i].color);
        }

        // Render the scene
        SDL_RenderPresent(renderer);
        SDL_Delay(FRAMERATE_DELAY); // Add a small delay to control the frame rate
    }

    // Cleanup and quit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

