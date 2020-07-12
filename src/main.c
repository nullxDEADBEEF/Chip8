#include <stdio.h>
#include <SDL2/SDL.h>
#include "main.h"
#include "cpu.h"

#define WIDTH 800
#define HEIGHT 600

SDL_Window *window;

int initialize_window() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WIDTH,
                              HEIGHT,
                              SDL_WINDOW_OPENGL);

    if (window == NULL) {
        printf("Error in creating window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Delay(3000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main(int argc, char* argv[]) {
    initialize_window();
    cpu_t cpu = cpu_create();
    cpu_print_status(&cpu);
    return 0;
}