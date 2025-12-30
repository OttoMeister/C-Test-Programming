
// https://formatter.org/ WebKit format
// gcc -o test test7.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image -lSDL2_ttf -lm && ./test

#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
#define BALL_RADIUS 10
#define MAX_SPEED 2.0
#define NUM_BALLS 500
#define SEPARATION_RADIUS 40.0
#define ALIGNMENT_RADIUS 60.0
#define COHESION_RADIUS 80.0
#define MAX_FORCE 0.2
#define SEPARATION_FORCE 5
#define FRAMERATE_DELAY 0


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

// Function to draw a triangle on the SDL renderer
void drawTriangle(SDL_Renderer* renderer, double x, double y, double angle, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    // Define the vertices of the triangle
    double vertices[3][2] = {
        {x + cos(angle) * BALL_RADIUS, y + sin(angle) * BALL_RADIUS},
        {x + cos(angle + (2 * M_PI / 3)) * BALL_RADIUS, y + sin(angle + (2 * M_PI / 3)) * BALL_RADIUS},
        {x + cos(angle + (4 * M_PI / 3)) * BALL_RADIUS, y + sin(angle + (4 * M_PI / 3)) * BALL_RADIUS}
    };

    // Draw the filled triangle
    for (int i = 0; i < 3; i++) {
        SDL_RenderDrawLine(renderer, (int)vertices[i][0], (int)vertices[i][1], (int)vertices[(i + 1) % 3][0], (int)vertices[(i + 1) % 3][1]);
    }
}




// Function to draw a filled circle on the SDL renderer
void drawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color)
{
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
void flockingRules(Triangle balls[], int numBalls)
{
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

// Function to render smooth (antialiased) text using SDL_ttf
SDL_Texture* renderSmoothText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color)
{
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

int main(int argc, char* argv[])
{
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Event event;
  bool quit = false;

  int screenWidth = 800; // Initial screen width
  int screenHeight = 600; // Initial screen height

  // Seed the random number generator
  srand(time(NULL));

  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  // Create a resizable window
  window = SDL_CreateWindow("Flocking Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  // Create a renderer with antialiasing
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  // Initialize SDL_ttf
  TTF_Init();

  // Load the font with antialiasing
  TTF_Font* font = TTF_OpenFont(FONT_PATH, 28); // Adjust the font size as needed
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // Enable antialiasing

  
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
            if (triangles[i].x < BALL_RADIUS || triangles[i].x > screenWidth - BALL_RADIUS) {
                triangles[i].vx = -triangles[i].vx;
            }

            if (triangles[i].y < BALL_RADIUS || triangles[i].y > screenHeight - BALL_RADIUS) {
                triangles[i].vy = -triangles[i].vy;
            }

            // Draw the triangle
            drawTriangle(renderer, triangles[i].x, triangles[i].y, triangles[i].angle, triangles[i].color);
        }

    // Render smooth (antialiased) text and display it on the screen
    SDL_Color textColor = { 255, 255, 255 }; // White color for the text
    const char* text = "Flocking Balls"; // The text you want to display
    SDL_Texture* textTexture = renderSmoothText(renderer, font, text, textColor);
    if (textTexture) {
      SDL_Rect textRect = { 10, 10, 200, 50 }; // Adjust the position and size of the text
      SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
      SDL_DestroyTexture(textTexture);
    }

    // Render the scene
    SDL_RenderPresent(renderer);
    SDL_Delay(FRAMERATE_DELAY); // Add a small delay to control the frame rate
  }

  // Cleanup and quit
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

