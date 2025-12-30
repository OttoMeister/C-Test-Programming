// https://tcc.lovebyte.party/day8extra/
// cd ~/CodePlay && time gcc -o test src/lovebyte4.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lm && timeout 20s ./test &
// cd ~/CodePlay && time gcc -o test src/lovebyte4.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm && timeout 20s ./test &

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>

int main() {
  int const n = 40, m = 200;
  float const r = 0.1;
  float x = 0, v = 0, t = 0;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  int w = 600, h = 600;  
  SDL_Window* window = SDL_CreateWindow("LoveByte", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  int quit = 0;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = 1;
      } else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          // Update the window size
          w = event.window.data1;
          h = event.window.data2;
        }
      }
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the pattern
    for (int i = 0; i <= n; i++) {
      for (int j = 0; j <= m; j++) {
        float u = sin(i + v) + sin(r * i + x);
        v = cos(i + v) + cos(r * i + x);
        x = u + t;

        // Calculate pixel coordinates
        int px = u * w / 4 + w / 2;
        int py = v * h / 4 + h / 2;

        // Set color and draw the point
        SDL_SetRenderDrawColor(renderer, (i * 255) / n, (j * 255) / m, 99, 255);
        SDL_RenderDrawPoint(renderer, px , py );         
      }
    }

    // Update time
    t += 0.001;

    // Update the screen
    SDL_RenderPresent(renderer);

    // Add a small delay
    SDL_Delay(20);
  }

  // Clean up SDL
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

