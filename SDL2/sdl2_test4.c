#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> // for sqrt

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BALL_RADIUS 10
#define MAX_BALL_SPEED 5
#define NUM_BALLS 50  // Number of balls to display

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    SDL_Color color;
} Ball;

void initializeBalls(Ball balls[], int numBalls) {
    for (int i = 0; i < numBalls; i++) {
        balls[i].x = rand() % (SCREEN_WIDTH - BALL_RADIUS);
        balls[i].y = rand() % (SCREEN_HEIGHT - BALL_RADIUS);
        balls[i].dx = (rand() % (2 * MAX_BALL_SPEED + 1)) - MAX_BALL_SPEED;
        balls[i].dy = (rand() % (2 * MAX_BALL_SPEED + 1)) - MAX_BALL_SPEED;
        balls[i].color.r = rand() % 256;
        balls[i].color.g = rand() % 256;
        balls[i].color.b = rand() % 256;
    }
}

void drawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

bool checkCollision(int x1, int y1, int x2, int y2, int radius1, int radius2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int distanceSquared = dx * dx + dy * dy;
    int radiiSumSquared = (radius1 + radius2) * (radius1 + radius2);

    return distanceSquared <= radiiSumSquared;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool quit = false;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL));

    Ball balls[NUM_BALLS];
    initializeBalls(balls, NUM_BALLS);

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < NUM_BALLS; i++) {
            balls[i].x += balls[i].dx;
            balls[i].y += balls[i].dy;

            // Bounce off the screen edges
            if (balls[i].x < 0 || balls[i].x > SCREEN_WIDTH - BALL_RADIUS * 2) {
                balls[i].dx = -balls[i].dx;
            }
            if (balls[i].y < 0 || balls[i].y > SCREEN_HEIGHT - BALL_RADIUS * 2) {
                balls[i].dy = -balls[i].dy;
            }

            // Draw round balls
            drawFilledCircle(renderer, balls[i].x + BALL_RADIUS, balls[i].y + BALL_RADIUS, BALL_RADIUS, balls[i].color);

            // Check for collisions with other balls
            for (int j = 0; j < NUM_BALLS; j++) {
                if (i != j && checkCollision(balls[i].x + BALL_RADIUS, balls[i].y + BALL_RADIUS, balls[j].x + BALL_RADIUS, balls[j].y + BALL_RADIUS, BALL_RADIUS, BALL_RADIUS)) {
                    // Handle the collision by reversing the direction of both balls
                    int temp_dx = balls[i].dx;
                    int temp_dy = balls[i].dy;
                    balls[i].dx = balls[j].dx;
                    balls[i].dy = balls[j].dy;
                    balls[j].dx = temp_dx;
                    balls[j].dy = temp_dy;
                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

