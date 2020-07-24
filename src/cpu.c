#include <stdio.h>
#include <string.h>
#include "cpu.h"

#define NUM_FONT_ELEMENTS 80
#define PIXELS_IN_ROW 8

#define REGISTER_X_NIBBLE(X) ((X & 0x0F00) >> 8)
#define REGISTER_Y_NIBBLE(Y) ((Y & 0x00F0) >> 4)
// 8 bits 2^8 = 256
#define IMMEDIATE_NUMBER(N) (N & 0x00FF)
// 12 bits 2^12 = 4095
#define IMMEDIATE_MEMORY_ADDRESS(N) (N & 0x0FFF)

const uint8_t font[NUM_FONT_ELEMENTS] = {
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
    cpu_t cpu = {{0}};

    for (int i = 0; i < NUM_FONT_ELEMENTS; i++) {
        cpu.memory[i] = font[i];
    }

    // most programs in Chip8 start at 0x200
    cpu.program_counter = 0x200;
    cpu.isRunning = true;

    return cpu;
}

bool cpu_load_rom(cpu_t *cpu, char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("could not read file: %s\n", file_path);
        return false;
    }

    // get file size in bytes
    fseek(file, 0, SEEK_END);
    long size_in_bytes = ftell(file);
    rewind(file);

    // copy file contents into memory
    uint8_t file_contents[size_in_bytes];
    for (int i = 0; i < sizeof(file_contents); i++) {
        file_contents[i] = getc(file);
        //printf("%#X ", file_contents[i]);
    }

    memcpy(cpu->memory + 0x200, file_contents, sizeof(file_contents));

    fclose(file);
    return true;
}

uint16_t cpu_fetch_instruction(cpu_t *cpu) {
    // since an instruction is two bytes, we want to to store at all in the 
    // single opcode's bits, so we first move 8 bits left(which adds 8 zeroes)
    // then we bitwise OR to get the next 8 bits(pc + 1) merged with the 
    // earlier shifted bits 
    uint16_t result = cpu->memory[cpu->program_counter] << 8 | cpu->memory[cpu->program_counter + 1];
    cpu->program_counter += INSTRUCTION_SIZE_IN_BYTES;
    return result;
}

void cpu_decode_execute(cpu_t *cpu) {
    // NOTE: we can make things run more smoothly if
    // we only redraw screen when we modify display data
    cpu->current_opcode = cpu_fetch_instruction(cpu);

    // get first 4 bits
    switch(cpu->current_opcode & 0xF000) {
        // since both 0x0000 and 0x000E start with 0x0
        // we need to compare the last 4 bits
        case 0x0000:
            switch(cpu->current_opcode & 0x000F) {
                // clear the screen
                case 0x0000:
                    memset(cpu->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
                    break;

                // return from a subroutine
                case 0x000E:
                    --cpu->stack_pointer;
                    cpu->program_counter = cpu->stack[cpu->stack_pointer];
                    break;
            }
            break;

        // 0x1NNN -> jump to address NNN
        case 0x1000:
            cpu->program_counter = IMMEDIATE_MEMORY_ADDRESS(cpu->current_opcode);
            break;

        // 0x2NNN -> execute subroutine at address NNN
        case 0x2000:
            cpu->stack[cpu->stack_pointer] = cpu->program_counter;
            ++cpu->stack_pointer;
            cpu->program_counter = IMMEDIATE_MEMORY_ADDRESS(cpu->current_opcode);
            break;
   
        // 0x3XNN -> skip the following instruction if the value of VX equals NN
        case 0x3000:
            if (cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] == IMMEDIATE_NUMBER(cpu->current_opcode)) {
                cpu->program_counter += INSTRUCTION_SIZE_IN_BYTES;
            }
            break;

        // 0x4XNN -> skip the following instruction if the value of VX is not equal to NN
        // TODO
        case 0x4000:
            if (cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] != IMMEDIATE_NUMBER(cpu->current_opcode)) {
                cpu->program_counter += INSTRUCTION_SIZE_IN_BYTES;
            }
            break; 
        // 0x5XY0 -> skip the following instruction if the value of VX is equal to the value of register VY
        // TODO
        case 0x5000:
            if (cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] == cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)]) {
                cpu->program_counter += INSTRUCTION_SIZE_IN_BYTES;
            }
            break;

        // 0x6XNN -> store number NN in register VX
        // TODO
        case 0x6000:
            cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] = IMMEDIATE_NUMBER(cpu->current_opcode);
            break;

        // 0x7XNN -> add value NN to register VX
        // TODO
        case 0x7000:
            cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] += IMMEDIATE_NUMBER(cpu->current_opcode);
            break;

        // 0x8XY0 -> store value of register VY in register VX
        // TODO
        case 0x8000:
            switch(cpu->current_opcode & 0x000F) {
                // 0x8XY1 -> set VX to VX OR VY
                 case 0x0001:
                    cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] |= cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)];
                    break;

                // 0x8XY2 -> set VX to VX AND VY
                case 0x0002:
                    cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] &= cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)];
                    break;

                // 0x8XY3 -> set VX to VX XOR VY
                case 0x0003:
                    cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] ^= cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)];
                    break;

            }
            break;

        // 0x8XY4 -> add the value of register VY to register VX
        // set VF to 01 if a carry happens
        // set VF to 00 if a carry does not happen
        // TODO
        case 0x8004:
            break;

        // 0x8XY5 -> substract the value of register VY from register VX
        // set VF to 00 if a borrow happens
        // set VF to 01 if a borrow does not happen
        // TODO
        case 0x8005:
            break;

        // 0x8XY6 -> store the value of register VY shifted right one bit in register VX
        // set register VF to the least significant bit prior to the shift
        // TODO
        case 0x8006:
            break;

        // 0x8XY7 -> set register VX to the value of VY minus VX
        // set VF to 00 if a borrow happens
        // set VF to 01 if a borrow does not happen
        // TODO
        case 0x8007:
            break;

        // 0x8XYE -> store the value of register VY shifted left one bit in register VX
        // set register VF to the most significant bit prior to the shift
        // TODO
        case 0x800E:
            break;
        // 0x9XY0 -> skip the following instruction if the value of register VX is not equal to the value of register VY
        // TODO
        case 0x9000:
            if (cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] != cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)]) {
                cpu->program_counter += INSTRUCTION_SIZE_IN_BYTES;
            }
            break;

        // 0xANNN -> store memory address NNN in register I
        // TODO
        case 0xA000:
            cpu->I = IMMEDIATE_MEMORY_ADDRESS(cpu->current_opcode);
            break;
/*
        // 0xBNNN -> jump to address NNN + V0
        // TODO
        case 0xB000:
            break;

        // 0xCXNN -> set VX to random number with a mask of NN
        // TODO
        case 0xC000:
            break;
*/
        // 0xDXYN -> draw a sprite at position VX, VY with N bytes of sprite data
        // starting at the address stored in I
        // set VF to 01 if any changed pixels are changed to unset, and 00 otherwise
        case 0xD000: {
                uint16_t x = cpu->V[REGISTER_X_NIBBLE(cpu->current_opcode)] % DISPLAY_WIDTH;
                uint16_t y = cpu->V[REGISTER_Y_NIBBLE(cpu->current_opcode)] % DISPLAY_HEIGHT;
                cpu->V[0x000F] = 0;
                uint16_t height = cpu->current_opcode & 0x000F;
                for (int y_point = 0; y_point < height; y_point++) {
                    uint16_t sprite_data = cpu->memory[cpu->I + y_point];
                    for (int x_point = 0; x_point < PIXELS_IN_ROW; x_point++) {
                        // check if bits are active
                        // 0x80 -> 7th bit, since we are 0-based
                        if ((sprite_data & (0x80 >> x_point)) != 0) {
                            if (cpu->display[x + x_point + ((y + y_point) * DISPLAY_WIDTH)] == 1) {
                                cpu->V[0x000F] = 1;
                            }

                            // sets bits to 1 if there is a difference
                            // in bits
                            // by XOR'ing there is no need to have an instruction
                            // to erase the sprite data at a position
                            // we can erase simply by drawing on the same position with the same sprite
                            // this is why the VF register gets set to 1, to detect when a collision happens
                            // so if we have a pixel on the screen set to 01 and we want to draw a sprite containing
                            // 01 at the same pixel, the screen pixel gets turned off and the VF gets set
                            cpu->display[x + x_point + ((y + y_point) * DISPLAY_WIDTH)] ^= 1;
                        }
                    }
                }
            } 
            break;
/*
        // 0xEX9E -> skip the following instruction if the key corresponding to the hex value
        // currently stored in register VX is pressed
        // TODO
        case 0xE09E:
            break;

        // 0xEXA1 -> skip the following instruction if the key corresponding to the hex value
        // currently stored in register VX is not pressed
        // TODO
        case 0xE0A1:
            break;

        // 0xFX07 -> store the current value of the delay timer in register VX
        // TODO
        case 0xF007:
            break;

        // 0xFX0A -> wait for a keypress and store the result in register VX
        // TODO
        case 0xF00A:
            break;

        // 0xFX15 -> set the delay timer to the value of register VX
        // TODO
        case 0xF015:
            break;
        
        // 0xFX18 -> set the sound timer to the value of register VX
        // TODO
        case 0xF018:
            break;

        // 0xFX1E -> add the value stored in register VX to register I
        // TODO
        case 0xF01E:
            break;
        
        // 0xFX29 -> set I to the memory address of the sprite data
        // corresponding to the hexadecimal digit stored in register VX
        // TODO
        case 0xF029:
            break;

        // 0xFX33 -> store the binary-coded hexadecimal equivalent of the value
        // stored in register VX at addresses I, I + 1, I + 2
        // TODO
        case 0xF033:
            break;

        // 0xFX55 -> store the values of register V0 to VX inclusive in memory starting
        // at address I, I is set to I + X + 1 after operation
        // TODO
        case 0xF055:
            break;

        // 0xFX65 -> fill registers V0 to VX inclusive with the values stored in 
        // memory starting at address I, I is set to I + X + 1 after operation
        // TODO
        case 0xF065:
            break;
*/
        default:
            printf("Opcode not recognized: %#X\n", cpu->current_opcode);
            break;
    }
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
