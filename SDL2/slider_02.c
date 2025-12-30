// gcc -o test src/slider_02.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm
// ps aux | grep  "[p]luma" | grep  -q "slider_02"  || pluma src/slider_02.c & gcc -o test src/slider_02.c -I/usr/include/SDL2 -D_REENTRAN -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm && timeout 20s ./test &
// https://formatter.org/ Coding Style Google - Column Limit 120

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("XP Style Slider", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int sliderValue = 50;
    int sliderMin = 1;
    int sliderMax = 100;

    int windowWidth = 600;
    int windowHeight = 600;

    int draggingSlider = 0;

    int quit = 0;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    windowWidth = e.window.data1;
                    windowHeight = e.window.data2;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;

                int handleXPosition = windowWidth / 4 + (float)(windowWidth / 2) * ((float)(sliderValue - sliderMin) / (float)(sliderMax - sliderMin)) - 10;
                if (mouseY >= windowHeight - 25 && mouseY <= windowHeight - 15 &&
                    mouseX >= handleXPosition && mouseX <= handleXPosition + 20) {
                    draggingSlider = 1;
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                draggingSlider = 0;
            } else if (e.type == SDL_MOUSEMOTION && draggingSlider) {
                int mouseX = e.motion.x;
                
                    // Add boundary checks for the slider handle
    if (mouseX < windowWidth / 4) {
        mouseX = windowWidth / 4;
    } else if (mouseX > windowWidth * 3 / 4) {
        mouseX = windowWidth * 3 / 4;
    }

                
                sliderValue = (int)((float)(sliderMax - sliderMin) * ((float)(mouseX - windowWidth / 4) / (float)(windowWidth / 2))) + sliderMin;
            }
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        int handleWidth = 20;
        int handleHeight = 10;
        int trackHeight = 5;

        boxRGBA(renderer, windowWidth / 4, windowHeight - 20 - trackHeight / 2, windowWidth * 3 / 4, windowHeight - 20 + trackHeight / 2, 200, 200, 200, 255);

        lineRGBA(renderer, windowWidth / 4, windowHeight - 20 - trackHeight / 2, windowWidth * 3 / 4, windowHeight - 20 - trackHeight / 2, 255, 255, 255, 255);
        lineRGBA(renderer, windowWidth / 4, windowHeight - 20 + trackHeight / 2, windowWidth * 3 / 4, windowHeight - 20 + trackHeight / 2, 150, 150, 150, 255);

        int handleXPosition = windowWidth / 4 + (float)(windowWidth / 2) * ((float)(sliderValue - sliderMin) / (float)(sliderMax - sliderMin)) - handleWidth / 2;

        boxRGBA(renderer, handleXPosition, windowHeight - 20 - handleHeight / 2, handleXPosition + handleWidth, windowHeight - 20 + handleHeight / 2, 200, 200, 200, 255);

        lineRGBA(renderer, handleXPosition, windowHeight - 20 - handleHeight / 2, handleXPosition + handleWidth, windowHeight - 20 - handleHeight / 2, 255, 255, 255, 255);
        lineRGBA(renderer, handleXPosition, windowHeight - 20 - handleHeight / 2, handleXPosition, windowHeight - 20 + handleHeight / 2, 255, 255, 255, 255);
        lineRGBA(renderer, handleXPosition + handleWidth, windowHeight - 20 - handleHeight / 2, handleXPosition + handleWidth, windowHeight - 20 + handleHeight / 2, 150, 150, 150, 255);
        lineRGBA(renderer, handleXPosition, windowHeight - 20 + handleHeight / 2, handleXPosition + handleWidth, windowHeight - 20 + handleHeight / 2, 150, 150, 150, 255);

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
