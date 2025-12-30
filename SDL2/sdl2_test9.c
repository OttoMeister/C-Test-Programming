// Include necessary libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define constant values for flocking behavior
#define FLOCK_SIZE 10
#define MAX_SPEED 2.0
#define NUM_FLOCKS 500
#define SEPARATION_RADIUS 40.0
#define ALIGNMENT_RADIUS 60.0
#define COHESION_RADIUS 80.0
#define MAX_FORCE 0.2
#define SEPARATION_FORCE 3
#define FRAMERATE_DELAY 0

// Define a structure for flock properties
typedef struct {
  double x, y, angle, velocity_x, velocity_y;
  SDL_Color color;
} Flock;

// Function to initialize the flock with random values
void initializeFlocks(Flock flocks[], int numFlocks, int screenWidth, int screenHeight) {
  for (int i = 0; i < numFlocks; i++) {
    // Random values for initial position, velocity, and color
    flocks[i] = (Flock){.x = rand() % screenWidth,
                        .y = rand() % screenHeight,
                        .angle = ((double)rand() / RAND_MAX) * 2 * M_PI,
                        .velocity_x = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
                        .velocity_y = ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED,
                        .color = {rand() % 256, rand() % 256, rand() % 256}};
  }
}

// Function to draw a flock on the renderer
void drawFlock(SDL_Renderer* renderer, double x, double y, double angle, SDL_Color color) {
  // Draw the flock as a simple line drawing
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
  double length = FLOCK_SIZE, width = FLOCK_SIZE / 3;
  double tip_x = x + cos(angle) * length, tip_y = y + sin(angle) * length;
  double beak1_x = x + cos(angle + (5 * M_PI / 6)) * width, beak1_y = y + sin(angle + (5 * M_PI / 6)) * width;
  double beak2_x = x + cos(angle + (7 * M_PI / 6)) * width, beak2_y = y + sin(angle + (7 * M_PI / 6)) * width;

  SDL_RenderDrawLine(renderer, (int)x, (int)y, (int)tip_x, (int)tip_y);
  SDL_RenderDrawLine(renderer, (int)tip_x, (int)tip_y, (int)beak1_x, (int)beak1_y);
  SDL_RenderDrawLine(renderer, (int)tip_x, (int)tip_y, (int)beak2_x, (int)beak2_y);
}

// Function that calculates the flocking rules for the group 
void applyFlockingRules(Flock flocks[], int numFlocks) {
  // Parameters for rule ranges and maximum forces
  double separationRadius = SEPARATION_RADIUS, alignmentRadius = ALIGNMENT_RADIUS, cohesionRadius = COHESION_RADIUS, maxForce = MAX_FORCE;

  for (int i = 0; i < numFlocks; i++) {
    // Initialize rule forces and counters
    double separationX = 0.0, separationY = 0.0, alignmentX = 0.0, alignmentY = 0.0, cohesionX = 0.0, cohesionY = 0.0;
    int separationCount = 0, alignmentCount = 0, cohesionCount = 0;

    for (int j = 0; j < numFlocks; j++) {
      if (i != j) {
        // Calculate the distance between the elements of the flock
        double dx = flocks[j].x - flocks[i].x, dy = flocks[j].y - flocks[i].y, distance = sqrt(dx * dx + dy * dy);

        // Separation rule: Avoid collisions
        if (distance < separationRadius) {
          separationX -= dx / (distance / SEPARATION_FORCE);
          separationY -= dy / (distance / SEPARATION_FORCE);
          separationCount++;
        }

        // Alignment rule: Align with neighbors
        if (distance < alignmentRadius) {
          alignmentX += flocks[j].velocity_x;
          alignmentY += flocks[j].velocity_y;
          alignmentCount++;
        }

        // Cohesion rule: Cohesion with neighbors
        if (distance < cohesionRadius) {
          cohesionX += flocks[j].x;
          cohesionY += flocks[j].y;
          cohesionCount++;
        }
      }
    }

    // Calculate average rule forces
    if (separationCount > 0) separationX /= separationCount, separationY /= separationCount;
    if (alignmentCount > 0) alignmentX /= alignmentCount, alignmentY /= alignmentCount;
    if (cohesionCount > 0) {
      cohesionX /= cohesionCount;
      cohesionY /= cohesionCount;
      cohesionX = (cohesionX - flocks[i].x) / 100.0;
      cohesionY = (cohesionY - flocks[i].y) / 100.0;
    }

    // Calculate total force and limit it to maximum force
    double totalForceX = separationX + alignmentX + cohesionX;
    double totalForceY = separationY + alignmentY + cohesionY;
    double magnitude = sqrt(totalForceX * totalForceX + totalForceY * totalForceY);

    if (magnitude > maxForce) {
      totalForceX = (totalForceX / magnitude) * maxForce;
      totalForceY = (totalForceY / magnitude) * maxForce;
    }

    // Update velocity while considering maximum speed
    flocks[i].velocity_x += totalForceX;
    flocks[i].velocity_y += totalForceY;
    double speed = sqrt(flocks[i].velocity_x * flocks[i].velocity_x + flocks[i].velocity_y * flocks[i].velocity_y);

    if (speed > MAX_SPEED) {
      flocks[i].velocity_x = (flocks[i].velocity_x / speed) * MAX_SPEED;
      flocks[i].velocity_y = (flocks[i].velocity_y / speed) * MAX_SPEED;
    }
      // Update each element of the flock position
      flocks[i].x += flocks[i].velocity_x, flocks[i].y += flocks[i].velocity_y;

    // Update movement angle
    flocks[i].angle = atan2(flocks[i].velocity_y, flocks[i].velocity_x);
  }
}

int main(int argc, char* argv[]) {
  // Initialize the SDL library and create a window and renderer
  // SDL window
  SDL_Window* window = NULL;
  // SDL renderer
  SDL_Renderer* renderer = NULL;
  // SDL event handling
  SDL_Event event;
  // Quit the main program if true
  bool quit = false;
  // Initial window size
  int screenWidth = 800, screenHeight = 600;

  // Initialize SDL video functionality
  SDL_Init(SDL_INIT_VIDEO);
  // Create the window
  window = SDL_CreateWindow("Flocking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  // Create the renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  // Set render quality hint
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  if (renderer == NULL) {
    fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Initialize the random number generator
  srand(time(NULL));
  // Create an array of flock objects
  Flock flock[NUM_FLOCKS];
  // Initialize flock positions
  initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = true;  // Quit the program on close event
      else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          // Update window size
          screenWidth = event.window.data1, screenHeight = event.window.data2;
          // Update +flock positions
          initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);
        }
      }
    }
    // Set background color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    // Clear the renderer
    SDL_RenderClear(renderer);

    // Calculate flocking rules for the flock
    applyFlockingRules(flock, NUM_FLOCKS);
    for (int i = 0; i < NUM_FLOCKS; i++) {
      // Change direction on collision with window edge
      if (flock[i].x < FLOCK_SIZE || flock[i].x > screenWidth - FLOCK_SIZE) flock[i].velocity_x = -flock[i].velocity_x;
      if (flock[i].y < FLOCK_SIZE || flock[i].y > screenHeight - FLOCK_SIZE) flock[i].velocity_y = -flock[i].velocity_y;
      // Draw each element of the flock on the renderer
      drawFlock(renderer, flock[i].x, flock[i].y, flock[i].angle, flock[i].color);
    }

    // Update the renderer
    SDL_RenderPresent(renderer);
    // Delay for frame rate
    SDL_Delay(FRAMERATE_DELAY);
  }
  // Destroy the renderer
  SDL_DestroyRenderer(renderer);
  // Destroy the window
  SDL_DestroyWindow(window);
  // Quit the SDL library
  SDL_Quit();
  // Exit the program successfully
  return 0;
}
