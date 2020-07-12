#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "cpu.h"

#define WIDTH 800
#define HEIGHT 600

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int initialize_window() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WIDTH,
                              HEIGHT,
                              SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (window == NULL) {
        printf("Error in creating window: %s\n", SDL_GetError());
        return 1;
    }

    if (renderer == NULL) {
        printf("Error in creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    return 0;
}

void handle_window_input(cpu_t *cpu) {
    while(cpu->isRunning) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                cpu->isRunning = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
}

int main(int argc, char* argv[]) {
    cpu_t cpu = cpu_create();
    initialize_window();
    handle_window_input(&cpu);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}