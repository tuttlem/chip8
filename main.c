
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "machine.h"
#include "interface.h"


int main(int argc, char *argv[]) {
    uint8_t interrupted = 0;
    struct chip8_machine *m = chip8_machine_create();
    chip8_load(m, "../programs/pong.ch8");

    interface_init();

    while (!interrupted) {
        if (interface_capture_events(m) < 0) {
            interrupted = 1;
        }

        chip8_step(m);
        interface_draw(m);

        SDL_Delay(1);
    }

    interface_destroy();
    chip8_machine_destroy(&m);

    return 0;

}
