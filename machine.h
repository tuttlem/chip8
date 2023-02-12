#ifndef __chip8_machine_h_

#define __chip8_machine_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <SDL2/SDL.h>

#define VIDEO_WIDTH     64
#define VIDEO_HEIGHT    32

/*
 System memory map
 +---------------+= 0xFFF (4095) End Chip-8 RAM
 |               |
 |               |
 |               |
 |               |
 |               |
 | 0x200 to 0xFFF|
 |     Chip-8    |
 | Program / Data|
 |     Space     |
 |               |
 |               |
 |               |
 +- - - - - - - -+= 0x600 (1536) Start ETI 660 Chip-8 programs
 |               |
 |               |
 |               |
 +---------------+= 0x200 (512) Start of most Chip-8 programs
 | 0x000 to 0x1FF|
 | Reserved for  |
 |  interpreter  |
 +---------------+= 0x000 (0) Begin Chip-8 RAM. We store font data here instead of storing the interpreter because we don't have that restriction.
*/

/**
 * Chip8 machine structure
 */
struct chip8_machine {
    uint8_t memory[0x1000];          /* 4k block of memory */
    uint8_t v[16];                   /* 16 general registers */
    uint16_t i;                      /* index register */
    uint16_t pc;                     /* program counter */
    uint16_t stack[16];              /* machine stack */
    uint16_t sp;                     /* stack pointer */
    uint8_t delay_timer;             /* delay counter */
    uint8_t sound_timer;             /* sound beep timer */
    uint8_t key[16];                 /* key states */
    uint8_t display[(VIDEO_WIDTH * VIDEO_HEIGHT)]; /* display memory */

    uint8_t draw_flag;               /* determines if the screen needs to be redrawn */
    uint8_t beep_flag;               /* determines if the beep needs to be played */
};

/**
 * Creates a new chip8 machine
 * @return {struct chip8_machine*} The newly created machine
 */
struct chip8_machine* chip8_machine_create(void);

/**
 * Destroys the given chip8 machine
 * @param {struct chip8_machine**} p The machine to destroy
 * @return {int} The outcome of the execution
 */
int chip8_machine_destroy(struct chip8_machine **p);

/**
 * Prints the given machine to stdout
 * @param {struct chip8_machine*} m The machine to print
 */
void chip8_print(struct chip8_machine *m);

/**
 * Executes the given opcode on the machine
 * @param {struct chip8_machine*} m The machine to execute the opcode on
 * @param {uint16_t} opcode The opcode to execute
 * @return {int} The outcome of the execution
 */
int chip8_execute(struct chip8_machine *m, uint16_t opcode);

/**
 * Loads a chip8 program into the machine
 * @param {struct chip8_machine*} m The machine to load the program into
 * @param {const char *} path The path to the program to load
 * @return {int} The outcome of the execution
 */
int chip8_load(struct chip8_machine *m, const char *path);

/**
 * Performs one execution cylce on the machine
 * @param {struct chip8_machine*} m The machine to progress step execution
 * @return {int} The outcome of the execution
 */
int chip8_step(struct chip8_machine *m);

#endif /* __chip8_machine_h_ */