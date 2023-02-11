
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "machine.h"
#include "interface.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define VIDEO_WIDTH     64
#define VIDEO_HEIGHT    32

const int FPS = 100;
const int DELAY_TIME = 1000000 / FPS;

void bit_print(uint8_t n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}



int main(int argc, char *argv[]) {
    struct chip8_machine *m = chip8_machine_create();
    chip8_load(m, "../programs/pong.ch8");

    interface_init();

    uint8_t interrupted = 0;
    clock_t current_time = clock();

    while (!interrupted) {
        if (interface_capture_events(m) < 0) {
            interrupted = 1;
        }

        chip8_step(m);

        if (interface_draw(m)) {
            usleep(16666);
        }


    }

    interface_destroy();
    chip8_machine_destroy(&m);

    return 0;

}
