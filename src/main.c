#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "cpu.h"

#define WIDTH 128
#define HEIGHT 64

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool window_create() {
    uint8_t pixel_scale = 16;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Could not init SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    window = SDL_CreateWindow("Chip-8 Intepreter",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WIDTH * (pixel_scale / 2),
                              HEIGHT * (pixel_scale / 2),
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

    SDL_RenderSetScale(renderer, pixel_scale, pixel_scale);

    return true;
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
            // draw if bit is active
            if (cpu->display[j + (i * DISPLAY_WIDTH)] != 0) {
                SDL_RenderDrawPoint(renderer, j, i);
            }
        }
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
}

int main(int argc, char* argv[]) {
    // checking if a ROM file was specificed
    if (argc < 2 || argc > 2) {
        printf("Usage: ./chip8.out <ROM>\n");
        return -1;
    }
    cpu_t cpu = cpu_create();
    // load rom into memory
    cpu_load_rom(&cpu, argv[1]);

    window_create();

    while (cpu.isRunning) {
        cpu_decode_execute(&cpu);
        window_process_input(&cpu);
        window_render(&cpu);
        SDL_Delay(1 / 600);
    }

    //cpu_print_status(&cpu);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
