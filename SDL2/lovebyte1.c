// https://tcc.lovebyte.party/day8extra/
// gcc -o test lovebyte.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lm && ./test
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define W 600
#define H 600

int main() {
    int n = 40, m = 200;
    float r = 2 * M_PI / 235.0, x = 0, v = 0, t = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("LoveByte", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int q = 0;
    while (!q) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) q = 1;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        for (int i = 0; i <= n; i++)
            for (int j = 0; j <= m; j++) {
                float a = i + v;
                float b = r * i + x;
                float u = sin(a) + sin(b);
                v = cos(a) + cos(b);
                x = u + t;
                int px = u * W / 4 + W / 2;
                int py = v * H / 4 + H / 2;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawPoint(renderer, px, py);
            }
        
        SDL_RenderPresent(renderer);
        t += 0.01;
        SDL_Delay(20);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

