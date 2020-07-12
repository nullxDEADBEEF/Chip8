#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "cpu.h"

cpu_t cpu_create() {
    cpu_t cpu;

    memset(cpu.memory, 0, MAX_MEMORY);
    cpu.current_opcode = 0;
    memset(cpu.V, 0, NUM_REGISTERS);
    cpu.I = 0;
    cpu.delay_timer = 60;
    cpu.sound_timer = 60;
    cpu.program_counter = 0x200;
    cpu.stack_pointer = 0;
    memset(cpu.stack, 0, STACK_SIZE - 1);
    memset(cpu.key, 0, NUM_KEYS);
    memset(cpu.display, 0, DISPLAY_SIZE);

    return cpu;
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
