#include "machine.h"

uint8_t font_data[] = {
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


struct chip8_machine* chip8_machine_create(void) {
    struct chip8_machine *m = malloc(sizeof(struct chip8_machine));

    if (m == NULL) {
        return NULL;
    }

    // start with a zero-ed machine
    memset(m, 0, sizeof(struct chip8_machine));
    memcpy(m->memory, font_data, sizeof(font_data));

    srand(clock());

    return m;
}

int chip8_machine_destroy(struct chip8_machine **p) {
    if (p == NULL || *p == NULL) {
        return -1;
    }

    free(*p);
    *p = NULL;
    return 0;
}

void chip8_print(struct chip8_machine *m) {
    if (m == NULL) {
        return;
    }

    printf("pc: 0x%04X, i: 0x%04X, sp: 0x%02X, dt: 0x%02X, st: 0x%02X \n",
           m->pc,
           m->i,
           m->sp,
           m->delay_timer,
           m->sound_timer
   );
}

/**
 * Loads a chip8 program into the machine
 * @param {struct chip8_machine*} m The machine to load the program into
 * @param {const char *} path The path to the program to load
 * @return {int} The outcome of the execution
 */
int chip8_load(struct chip8_machine *m, const char *filename) {
    FILE *f = fopen(filename, "rb");

    if (!f) {
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    fread(m->memory + 0x200, fsize, 1, f);
    fclose(f);

    m->pc = 0x200;

    return 1;
}
