#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define MAX_MEMORY 4096
#define STACK_SIZE 16
#define NUM_KEYS 16
#define NUM_REGISTERS 16
#define DISPLAY_SIZE (64 * 32)

typedef enum {false = 0, true = 1} bool;

typedef struct {
    uint8_t memory[MAX_MEMORY];
    uint16_t current_opcode;
    uint8_t V[NUM_REGISTERS]; // V0 - VF
    uint16_t I; // used to store memory addresses
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t program_counter; // 0x00 - 0xFFF (0 - 4095), stores currently executing address
    uint8_t stack_pointer; // used to point to the topmost level in the stack
    uint16_t stack[STACK_SIZE]; // 
    uint8_t key[NUM_KEYS];
    uint8_t display[DISPLAY_SIZE];
    bool isRunning;
} cpu_t;

cpu_t cpu_create();
void cpu_print_status(cpu_t *cpu);
int cpu_load_rom(char *file_path);


#endif // CPU_H