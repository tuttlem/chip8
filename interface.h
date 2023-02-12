#ifndef __chip8_interface_h_

#define __chip8_interface_h_

#include <signal.h>
#include <ncurses.h>
#include <unistd.h>
#include <math.h>
#include "machine.h"

#define AMPLITUDE           28000
#define FREQUENCY           440
#define SAMPLE_RATE         44100
#define SAMPLES             44100
#define CHANNELS            2
#define BYTES_PER_SAMPLE    2
#define AUDIO_BUFFER_SIZE   (SAMPLES * BYTES_PER_SAMPLE * CHANNELS)

int interface_init(void);

int interface_destroy(void);

int interface_draw(struct chip8_machine *m);

int interface_capture_events(struct chip8_machine *m);

#endif /* __chip8_interface_h_ */