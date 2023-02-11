#ifndef __chip8_interface_h_

#define __chip8_interface_h_

#include <signal.h>
#include <ncurses.h>
#include <unistd.h>
#include "machine.h"

int interface_init(void);

int interface_destroy(void);

int interface_draw(struct chip8_machine *m);

int interface_capture_events(struct chip8_machine *m);

#endif /* __chip8_interface_h_ */