
// gcc -o test test10.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image
// -lSDL2_ttf -lSDL2_gfx  -lm && ./test https://formatter.org/ Coding Style
// Google - Column Limit 120 https://chat.openai.com/

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
#define FISH_SIZE 20
#define MAX_SPEED 2.0
#define NUM_FLOCKS 100
#define SEPARATION_RADIUS 40.0
#define ALIGNMENT_RADIUS 60.0
#define COHESION_RADIUS 80.0
#define MAX_FORCE 0.2
#define SEPARATION_FORCE 5.0
#define FRAMERATE_DELAY 0

// Free Picture from https://unsplash.com/de/fotos/IBrZ-TXYWY8
#define BACKGROUND_PNG "aquarium.png"
// selfmade of sad fish :(
#define FISH_PNG "fish.png"

// Define a structure for flock properties
typedef struct {
  double x, y, angle, velocity_x, velocity_y;
} Fish;

// Function to initialize the flock with random values
void initializeFlocks(Fish flocks[], int numFlocks, int screenWidth,
                      int screenHeight) {
  for (int i = 0; i < numFlocks; i++) {
    // Random values for initial position and velocity
    flocks[i].x = rand() % screenWidth;
    flocks[i].y = rand() % screenHeight;
    flocks[i].angle = ((double)rand() / RAND_MAX) * 2 * M_PI;
    flocks[i].velocity_x =
        ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED;
    flocks[i].velocity_y =
        ((double)rand() / RAND_MAX) * 2 * MAX_SPEED - MAX_SPEED;
  }
}

// Function to draw a fish
void drawFlock(SDL_Renderer* renderer, double x, double y, double angle,
               SDL_Texture* birdTexture) {
  SDL_Rect dstRect;
  SDL_QueryTexture(birdTexture, NULL, NULL, &dstRect.w, &dstRect.h);

  // Set the position and angle for rendering the bird
  dstRect.x = (int)x;
  dstRect.y = (int)y;

  int newSize = FISH_SIZE * 2;
  dstRect.w = newSize;
  dstRect.h = newSize;

  // Rotate the bird image
  SDL_RenderCopyEx(renderer, birdTexture, NULL, &dstRect, angle * 180 / M_PI,
                   NULL, SDL_FLIP_NONE);
}

// Function that calculates the flocking rules for the group
void applyFlockingRules(Fish flocks[], int numFlocks, int screenWidth,
                        int screenHeight) {
  // Parameters for rule ranges and maximum forces

  for (int i = 0; i < numFlocks; i++) {
    // Initialize rule forces and counters
    double separationX = 0.0, separationY = 0.0;
    double alignmentX = 0.0, alignmentY = 0.0;
    double cohesionX = 0.0, cohesionY = 0.0;
    int separationCount = 0, alignmentCount = 0, cohesionCount = 0;

    for (int j = 0; j < numFlocks; j++) {
      if (i != j) {
        // Calculate the distance between the elements of the flock
        double distance_X = flocks[j].x - flocks[i].x;
        double distance_Y = flocks[j].y - flocks[i].y;
        double distance =
            sqrt(distance_X * distance_X + distance_Y * distance_Y);

        // Separation rule: Avoid collisions
        if (distance < SEPARATION_RADIUS) {
          separationX -= distance_X / (distance / SEPARATION_FORCE);
          separationY -= distance_Y / (distance / SEPARATION_FORCE);
          separationCount++;
        }

        // Alignment rule: Align with neighbors
        if (distance < ALIGNMENT_RADIUS) {
          alignmentX += flocks[j].velocity_x;
          alignmentY += flocks[j].velocity_y;
          alignmentCount++;
        }

        // Cohesion rule: Cohesion with neighbors
        if (distance < COHESION_RADIUS) {
          cohesionX += flocks[j].x;
          cohesionY += flocks[j].y;
          cohesionCount++;
        }
      }
    }

    // Calculate average separation direction
    if (separationCount > 0) {
      separationX /= separationCount;
      separationY /= separationCount;
    }

    // Calculate average alignment direction
    if (alignmentCount > 0) {
      alignmentX /= alignmentCount;
      alignmentY /= alignmentCount;
    }

    // Calculate average cohesion direction
    if (cohesionCount > 0) {
      cohesionX /= cohesionCount;
      cohesionY /= cohesionCount;

      // Move towards the center of mass, but at a slower pace
      cohesionX = (cohesionX - flocks[i].x) / 100.0;
      cohesionY = (cohesionY - flocks[i].y) / 100.0;
    }

    // Calculate total force and limit it to maximum force
    double totalForceX = separationX + alignmentX + cohesionX;
    double totalForceY = separationY + alignmentY + cohesionY;
    double magnitude =
        sqrt(totalForceX * totalForceX + totalForceY * totalForceY);

    if (magnitude > MAX_FORCE) {
      totalForceX = (totalForceX / magnitude) * MAX_FORCE;
      totalForceY = (totalForceY / magnitude) * MAX_FORCE;
    }

    // Update velocity while considering maximum speed
    flocks[i].velocity_x += totalForceX;
    flocks[i].velocity_y += totalForceY;
    double speed = sqrt(flocks[i].velocity_x * flocks[i].velocity_x +
                        flocks[i].velocity_y * flocks[i].velocity_y);

    if (speed > MAX_SPEED) {
      flocks[i].velocity_x = (flocks[i].velocity_x / speed) * MAX_SPEED;
      flocks[i].velocity_y = (flocks[i].velocity_y / speed) * MAX_SPEED;
    }
    // Update each element of the flock position
    flocks[i].x += flocks[i].velocity_x, flocks[i].y += flocks[i].velocity_y;

    // Update movement angle
    flocks[i].angle = atan2(flocks[i].velocity_y, flocks[i].velocity_x);

    // Change direction on collision with window edge

    if (flocks[i].x < -30) flocks[i].x = screenWidth + 20;
    if (flocks[i].x > screenWidth + 20) flocks[i].x = -30;
    if (flocks[i].y < -30) flocks[i].y = screenHeight + 20;
    if (flocks[i].y > screenHeight + 20) flocks[i].y = -30;
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
  window = SDL_CreateWindow("Flocking", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  // Create the renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
  Fish flock[NUM_FLOCKS];
  // Initialize flock positions
  initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);

  // Load fish image
  SDL_Surface* fishSurface = IMG_Load(FISH_PNG);
  if (fishSurface == NULL) {
    fprintf(stderr, "Failed to load fish image: %s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Texture* fishTexture =
      SDL_CreateTextureFromSurface(renderer, fishSurface);
  SDL_FreeSurface(fishSurface);

  // Load background image
  SDL_Surface* backgroundSurface = IMG_Load(BACKGROUND_PNG);
  if (backgroundSurface == NULL) {
    fprintf(stderr, "Failed to load background image: %s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Texture* backgroundTexture =
      SDL_CreateTextureFromSurface(renderer, backgroundSurface);
  SDL_FreeSurface(backgroundSurface);

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = true;  // Quit the program on close event
      else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          // Update window size
          screenWidth = event.window.data1, screenHeight = event.window.data2;
          // Update flock positions
          initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);
        }
      }
    }
    // Set background color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    // Clear the renderer
    SDL_RenderClear(renderer);

    // Draw background image
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Calculate flocking rules for the flock
    applyFlockingRules(flock, NUM_FLOCKS, screenWidth, screenHeight);

    for (int i = 0; i < NUM_FLOCKS; i++) {
      // Draw each element of the flock on the renderer
      drawFlock(renderer, flock[i].x, flock[i].y, flock[i].angle, fishTexture);
    }

    // Update the renderer
    SDL_RenderPresent(renderer);
    // Delay for frame rate
    SDL_Delay(FRAMERATE_DELAY);
  }

  // Destroy the texture
  SDL_DestroyTexture(fishTexture);
  SDL_DestroyTexture(backgroundTexture);
  // Destroy the renderer
  SDL_DestroyRenderer(renderer);
  // Destroy the window
  SDL_DestroyWindow(window);
  // Quit the SDL library
  SDL_Quit();
  // Exit the program successfully
  return 0;
}
