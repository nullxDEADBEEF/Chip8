#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "cpu.h"

#define WIDTH 800
#define HEIGHT 600

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int window_create() {
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
        return false;
    }

    if (renderer == NULL) {
        printf("Error in creating renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    return 0;
}

void window_process_input(cpu_t *cpu) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                cpu->isRunning = false;
                break;
            
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        cpu->isRunning = false;
                        break;
                }
        }
    }
}

void window_render(cpu_t *cpu) {
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
}

int main(int argc, char* argv[]) {
    cpu_t cpu = cpu_create();
    cpu_load_rom(&cpu, argv[1]);
    window_create();

    while (cpu.isRunning) {
        window_process_input(&cpu);
        window_render(&cpu);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}