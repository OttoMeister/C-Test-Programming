/*
Thanks to:
https://chat.openai.com/
https://formatter.org/ Coding Style Google - Column Limit 120

#Win10
https://winlibs.com/
https://www.mingw-w64.org/
cd %USERPROFILE%\Desktop\mingw64
SET PATH=%USERPROFILE%\Desktop\mingw64\bin;%PATH%
SET PATH=C:\Program Files\Notepad++;%PATH%
start notepad++ sdl2_test11.c
del test.exe
gcc -std=c17 sdl2_test11.c -L%USERPROFILE%\Desktop\mingw64\lib  -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -o test
start /B test.exe

#Linux
sudo apt install libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 -y
sudo apt install libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 -y

ps aux | grep  "[p]luma" | grep  -q "sdl2_test11"  || pluma src/sdl2_test11.c & gcc -o test src/sdl2_test11.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm && timeout 20s ./test &
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SPEED 5.0
#define NUM_FLOCKS 100
#define SEPARATION_RADIUS 40.0
#define ALIGNMENT_RADIUS 60.0
#define COHESION_RADIUS 80.0

#define COHESION_FACTOR 0.02
#define ALIGNMENT_FACTOR 0.01
#define SEPARATION_FACTOR 0.05

#define FRAMERATE_DELAY 5
#define FONT_SIZE 36
#define FONT_PATH "Atkinson.ttf"
#define BACKGROUND_PNG "aquarium.png"
#define FISH_PNG "fish.png"

typedef struct {
  float pos_x, pos_y, angle, velocity_x, velocity_y;
} Fish;

void initializeFlocks(Fish fish[], int numFlocks, int screenWidth, int screenHeight) {
  for (int i = 0; i < numFlocks; i++) {
    fish[i].pos_x = rand() % screenWidth;
    fish[i].pos_y = rand() % screenHeight;
    fish[i].angle = 0;
    fish[i].velocity_x = ((float)rand() / RAND_MAX) * 2.0 - 1.0;
    fish[i].velocity_y = ((float)rand() / RAND_MAX) * 2.0 - 1.0;
  }
}

void drawFish(SDL_Renderer* renderer, float x, float y, float angle, SDL_Texture* birdTexture) {
  SDL_Rect dstRect;
  SDL_QueryTexture(birdTexture, NULL, NULL, &dstRect.w, &dstRect.h);
  dstRect.x = (int)x;
  dstRect.y = (int)y;
  dstRect.w = 40;
  dstRect.h = 20;
  SDL_RenderCopyEx(renderer, birdTexture, NULL, &dstRect, angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
}

void applyFlockingRules(Fish fish[], int numFlocks, int screenWidth, int screenHeight) {
  for (int i = 0; i < numFlocks; i++) {
    float separationX = 0.0, separationY = 0.0;
    float alignmentX = 0.0, alignmentY = 0.0;
    float cohesionX = 0.0, cohesionY = 0.0;
    int separationCount = 0, alignmentCount = 0, cohesionCount = 0;
    for (int j = 0; j < numFlocks; j++) {
      if (i != j) {
        float distance_X = fish[j].pos_x - fish[i].pos_x;
        float distance_Y = fish[j].pos_y - fish[i].pos_y;
        float distance = sqrt(distance_X * distance_X + distance_Y * distance_Y);
        if (distance < SEPARATION_RADIUS) {
          separationX -= distance_X;
          separationY -= distance_Y;
          separationCount++;
        }
        if (distance < ALIGNMENT_RADIUS) {
          alignmentX += fish[j].velocity_x;
          alignmentY += fish[j].velocity_y;
          alignmentCount++;
        }
        if (distance < COHESION_RADIUS) {
          cohesionX += fish[j].pos_x;
          cohesionY += fish[j].pos_y;
          cohesionCount++;
        }
      }
    }
    if (separationCount > 0) {
      separationX /= separationCount;
      separationY /= separationCount;
    }
    if (alignmentCount > 0) {
      alignmentX /= alignmentCount;
      alignmentY /= alignmentCount;
    }
    if (cohesionCount > 0) {
      cohesionX /= cohesionCount;
      cohesionY /= cohesionCount;
      cohesionX = (cohesionX - fish[i].pos_x) / 100.0;
      cohesionY = (cohesionY - fish[i].pos_y) / 100.0;
    }
    float totalForceX = separationX + alignmentX + cohesionX;
    float totalForceY = separationY + alignmentY + cohesionY;
    // if (i==10) printf("ForeceX: %f,%f,%f ForceY: %f,%f,%f\n",separationX ,alignmentX , cohesionX, separationY ,alignmentY , cohesionY);
    
	// Geschwindigkeit normalisieren
    float speed = sqrt(totalForceX * totalForceX + totalForceY * totalForceY);
    if (speed > (MAX_SPEED/2)) {
      fish[i].velocity_x = (totalForceX / speed) * MAX_SPEED;
      fish[i].velocity_y = (totalForceY / speed) * MAX_SPEED;
    }
    // berechnung der neuen Position
    fish[i].pos_x += fish[i].velocity_x;
    fish[i].pos_y += fish[i].velocity_y;
    // Ausrichtung des Fisches
    fish[i].angle = atan2(fish[i].velocity_y, fish[i].velocity_x);
    // Grenze
    if (fish[i].pos_x < -30) fish[i].pos_x = screenWidth + 20;
    if (fish[i].pos_x > screenWidth + 20) fish[i].pos_x = -30;
    if (fish[i].pos_y < -30) fish[i].pos_y = screenHeight + 20;
    if (fish[i].pos_y > screenHeight + 20) fish[i].pos_y = -30;
  }
}

SDL_Texture* renderSmoothText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color) {
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

int main(int argc, char* argv[]) {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Event event;
  bool quit = false;
  int screenWidth = 800, screenHeight = 600;
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Flocking", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  TTF_Init();
  TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  if (!font) {
    fprintf(stderr, "TTF_OpenFont failed: %s\n", TTF_GetError());
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  srand(time(NULL));
  Fish flock[NUM_FLOCKS];
  initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);
  SDL_Surface* fishSurface = IMG_Load(FISH_PNG);
  if (fishSurface == NULL) {
    fprintf(stderr, "Failed to load fish image: %s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Texture* fishTexture = SDL_CreateTextureFromSurface(renderer, fishSurface);
  SDL_FreeSurface(fishSurface);
  SDL_Surface* backgroundSurface = IMG_Load(BACKGROUND_PNG);
  if (backgroundSurface == NULL) {
    fprintf(stderr, "Failed to load background image: %s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
  SDL_FreeSurface(backgroundSurface);
  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT)
        quit = true;
      else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          screenWidth = event.window.data1, screenHeight = event.window.data2;
          initializeFlocks(flock, NUM_FLOCKS, screenWidth, screenHeight);
        }
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_Color textColor = {255, 255, 255};
    const char* text = "Flocking Fish";
    SDL_Texture* textTexture = renderSmoothText(renderer, font, text, textColor);
    if (textTexture) {
      SDL_Rect textRect = {10, 10, 200, 50};
      SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
      SDL_DestroyTexture(textTexture);
    }
    applyFlockingRules(flock, NUM_FLOCKS, screenWidth, screenHeight);
    for (int i = 0; i < NUM_FLOCKS; i++) {
      drawFish(renderer, flock[i].pos_x, flock[i].pos_y, flock[i].angle, fishTexture);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(FRAMERATE_DELAY);
  }
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyTexture(fishTexture);
  SDL_DestroyTexture(backgroundTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
