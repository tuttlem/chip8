#include "interface.h"

WINDOW *back_buf, *front_buf;

void interface_resize_handler(int sig) {
    endwin();
    refresh();
}

int interface_init(void) {
    initscr();
    clear();
    noecho();
    cbreak();


    curs_set(FALSE);

    back_buf = newwin(VIDEO_HEIGHT, VIDEO_WIDTH, 0, 0);
    front_buf = newwin(VIDEO_HEIGHT, VIDEO_WIDTH, 0, 0);

    nodelay(stdscr, TRUE);

    signal(SIGWINCH, interface_resize_handler);

    return 0;
}

int interface_destroy(void) {
    endwin();
    return 0;
}

int interface_draw(struct chip8_machine *m) {
    int did_draw = 0;

    if (m->draw_flag) {
        m->draw_flag = 0;
        did_draw = 1;

        werase(back_buf);

        for (int i = 0; i < VIDEO_HEIGHT; i++) {
            for (int j = 0; j < VIDEO_WIDTH; j++) {
                mvwaddch(back_buf, i, j, m->display[j + (i * VIDEO_WIDTH)] ? '#' : ' ');
            }
        }

        for (int i = 0; i < 16; i ++) {
            mvwaddch(back_buf, 20, i, m->key[i] ? '#' : ' ');
        }

        overwrite(back_buf, front_buf);
        wrefresh(front_buf);
    }

    return did_draw;
}

int interface_capture_events(struct chip8_machine *m) {
    const int keymap[16] = {
        'x', '1', '2', '3',
        'q', 'w', 'e', 'a',
        's', 'd', 'z', 'c',
        '4', 'r', 'f', 'v'
    };

    int c = getch();

    if (c == 'p') {
        return -1;
    } else if (c == KEY_UP) {
        for (int i = 0; i < 16; i++) {
            if (c == keymap[i]) {
                m->key[i] = 0;
                break;
            }
        }
    } else {
        for (int i = 0; i < 16; i++) {
            if (c == keymap[i]) {
                m->key[i] = 1;
                break;
            }
        }
    }

    return 0;
}