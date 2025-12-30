// https://tcc.lovebyte.party/day8extra/
// gcc -o test lovebyte3.c -I/usr/include/SDL2 -D_REENTRANT -lSDL2 -lSDL2_gfx -lm && ./test
// ps aux | grep  "[p]luma" | grep  -q "slider_02"  || pluma src/lovebyte3.c & gcc -o test src/lovebyte3.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm && timeout 20s ./test &

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>

int main() {
    int  n = 40, m = 200;
    float const r = 0.1;
    float x = 0, v = 0, t = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("LoveByte", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize the slider parameters
    int sliderValue = 50;  // Initial slider value (adjust as needed)
    int sliderMin = 1;
    int sliderMax = 100;

    int windowWidth = 600;
    int windowHeight = 600;

    int draggingSlider = 0;  // Flag to check if slider is being dragged

    int q = 0;
    while (!q) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                q = 1;
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    // Update the window size
                    windowWidth = e.window.data1;
                    windowHeight = e.window.data2;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                q = 1;  // Quit on pressing the Escape key
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                // Check if the mouse click is within the slider range
                if (mouseY >= windowHeight - 30 && mouseY <= windowHeight - 10 &&
                    mouseX >= windowWidth / 4 && mouseX <= windowWidth * 3 / 4) {
                    draggingSlider = 1;
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                draggingSlider = 0;
            } else if (e.type == SDL_MOUSEMOTION && draggingSlider) {
                int mouseX = e.motion.x;
                // Update the slider position based on mouse movement
                sliderValue = (int)((float)(sliderMax - sliderMin) * ((float)(mouseX - windowWidth / 4) / (float)(windowWidth / 2))) + sliderMin;
            }
        }

        // Draw the slider
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        n = sliderValue;
        // Draw your existing graphics

        for (int i = 0; i <= n; i++)
            for (int j = 0; j <= m; j++) {
                float u = sin(i + v) + sin(r * i + x);
                v = cos(i + v) + cos(r * i + x);
                x = u + t;
                int px = u * windowWidth / 4 + windowWidth / 2;
                int py = v * windowHeight / 4 + windowHeight / 2;
                filledCircleRGBA(renderer, px, py, 1, (i * 255) / n, (j * 255) / m, 99, 255);
            }

        // Draw the slider
        thickLineRGBA(renderer, windowWidth / 4, windowHeight - 20, windowWidth * 3 / 4, windowHeight - 20, 10, 255, 255, 255, 255);
        filledCircleRGBA(renderer, windowWidth / 4 + (float)(windowWidth / 2) * ((float)(sliderValue - sliderMin) / (float)(sliderMax - sliderMin)), windowHeight - 20, 10, 255, 255, 255, 255);

        t += 0.01;
        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

