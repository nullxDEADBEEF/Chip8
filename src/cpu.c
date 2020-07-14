#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "cpu.h"

const uint8_t font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

cpu_t cpu_create() {
    cpu_t cpu;

    memset(cpu.memory, 0, MAX_MEMORY);
    cpu.current_opcode = 0;
    memset(cpu.V, 0, NUM_REGISTERS);
    cpu.I = 0;
    cpu.delay_timer = 60;
    cpu.sound_timer = 60;
    // most programs in Chip8 start at 0x200
    cpu.program_counter = 0x200;
    cpu.stack_pointer = 0;
    memset(cpu.stack, 0, STACK_SIZE - 1);
    memset(cpu.key, 0, NUM_KEYS);
    memset(cpu.display, 0, DISPLAY_SIZE);
    cpu.isRunning = true;

    return cpu;
}

int cpu_load_rom(cpu_t *cpu, char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("could not read file: %s\n", file_path);
        return -1;
    }

    // get file size in bytes
    fseek(file, 0, SEEK_END);
    long size_in_bytes = ftell(file);
    rewind(file);

    // copy file contents into memory
    uint8_t file_contents[size_in_bytes];
    for (int i = 0; i < sizeof(file_contents); i++) {
        file_contents[i] = getc(file);
        printf("%c ", file_contents[i]);
    }

    memcpy(cpu->memory + 0x200, file_contents, sizeof(file_contents));

    fclose(file);
    return 1;
}

void cpu_print_status(cpu_t *cpu) {
   for (int i = 0; i < MAX_MEMORY; i++) {
       printf("memory: %u ", cpu->memory[i]);
   } 

   printf("\n");

   for (int i = 0; i < NUM_REGISTERS; i++) {
       printf("\nV registers: %u ", cpu->V[i]);
   }

   printf("\n");

   for (int i = 0; i < STACK_SIZE; i++) {
       printf("\nstack: %u ", cpu->stack[i]);
   }

   printf("\n");

   for (int i = 0; i < NUM_KEYS; i++) {
       printf("\nkey: %u ", cpu->key[i]);
   }

   printf("\n\n");
   printf("cur opcode: %u\n", cpu->current_opcode);
   printf("I register: %u\n", cpu->I);
   printf("delay timer: %u\n", cpu->delay_timer);
   printf("sound timer: %u\n", cpu->sound_timer);
   printf("program counter: %u\n", cpu->program_counter);
   printf("stack pointer: %u\n", cpu->stack_pointer);

}
