#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// Linux
// sudo apt install libfreetype6-dev libsdl2-ttf-dev libsdl2-ttf-2.0-0 -y;
// sudo apt install libjpeg-dev libwebp-dev libtiff5-dev libsdl2-image-dev libsdl2-image-2.0-0 -y;
// gcc -o test test3.c `sdl2-config --cflags --libs` -lSDL2_mixer -lSDL2_image -lSDL2_ttf && ./test
#define LOADFONT "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"
#define LOADPNG "test.png"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) || !(TTF_Init() + 1)) {
        fprintf(stderr, "SDL or TTF initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Text on PNG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont(LOADFONT, 24);
    SDL_Color textColor = {255, 255, 255, 255}; // White

    SDL_Surface* imageSurface = IMG_Load(LOADPNG);
    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Hello SDL_ttf", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {50, 50, 0, 0};
    SDL_QueryTexture(textTexture, 0, 0, &textRect.w, &textRect.h);

    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e))
            quit = e.type == SDL_QUIT;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, imageTexture, 0, 0);
        SDL_RenderCopy(renderer, textTexture, 0, &textRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

