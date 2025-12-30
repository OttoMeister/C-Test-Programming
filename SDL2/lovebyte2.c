// https://tcc.lovebyte.party/day8extra/
// gcc -o test src/lovebyte2.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lm && ./test


#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

int main() {
  int const n = 40, m = 200;
  float const r = 0.1;
  //float const r = 2 * M_PI / 235.0;
  float x = 0, v = 0, t = 0;

  SDL_Init(SDL_INIT_VIDEO);
  int w = 600, h = 600;  
  SDL_Window* window = SDL_CreateWindow("LoveByte", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  int q = 0;
  while (!q) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        q = 1;
      } else if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          // Update the window size
          w = e.window.data1;
          h = e.window.data2;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    for (int i = 0; i <= n; i++)
      for (int j = 0; j <= m; j++) {
        float u = sin(i + v) + sin(r * i + x);
        v = cos(i + v) + cos(r * i + x);
        x = u + t;
        int px = u * w / 4 + w / 2;
        int py = v * h / 4 + h / 2;
        SDL_SetRenderDrawColor(renderer, (i * 255) / n, (j * 255) / m, 99, 255);
        for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
            SDL_RenderDrawPoint(renderer, px + dx, py + dy);
          }
        }
      }
    t += 0.01;
    SDL_RenderPresent(renderer);
    SDL_Delay(20);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

