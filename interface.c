#include "interface.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_AudioSpec spec, obtained;

uint8_t audio_buffer[AUDIO_BUFFER_SIZE];

void audio_callback(void *userdata, uint8_t *stream, int len) {
    for (int i = 0; i < len; i += BYTES_PER_SAMPLE * CHANNELS) {
        float time = (float)i / (BYTES_PER_SAMPLE * CHANNELS) / (float)SAMPLE_RATE;
        int sample = AMPLITUDE * sin(2 * M_PI * FREQUENCY * time);

        for (int channel = 0; channel < CHANNELS; ++channel) {
            *(int16_t*)&stream[i + channel * BYTES_PER_SAMPLE] = sample;
        }
    }
}

int interface_init(void) {
    window = SDL_CreateWindow(
            "Chip8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            VIDEO_WIDTH * 20,
            VIDEO_HEIGHT * 20,
            SDL_WINDOW_RESIZABLE
    );

    renderer = SDL_CreateRenderer(
            window,
            -1,
            0
    );

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            VIDEO_WIDTH,
            VIDEO_HEIGHT
    );

    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = CHANNELS;
    spec.samples = SAMPLES;
    spec.callback = audio_callback;
    spec.userdata = NULL;

    SDL_OpenAudio(&spec, &obtained);
    SDL_PauseAudio(1);

    return 0;
}

int interface_destroy(void) {
    SDL_Quit();
    return 0;
}

int interface_draw(struct chip8_machine *m) {
    if (m->draw_flag) {
        m->draw_flag = 0;

        uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
        for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
            pixels[i] = m->display[i] ? 0xFFFFFFFF : 0x00000000;

        }

        SDL_UpdateTexture(texture, NULL, pixels, VIDEO_WIDTH * sizeof(uint32_t));
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    /* TODO: come back to audio processing */
    /*
    const int status = SDL_GetAudioStatus();
    if (m->beep_flag == 1 && status != SDL_AUDIO_PLAYING) {
        SDL_PauseAudio(0);

    } else if (m->beep_flag == 0 && status == SDL_AUDIO_PLAYING) {
        SDL_PauseAudio(1);
    }
    */

    return 0;
}

int interface_capture_events(struct chip8_machine *m) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: return -1; break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_1: m->key[0x1] = event.type == SDL_KEYDOWN; break;
                    case SDLK_2: m->key[0x2] = event.type == SDL_KEYDOWN; break;
                    case SDLK_3: m->key[0x3] = event.type == SDL_KEYDOWN; break;
                    case SDLK_4: m->key[0xC] = event.type == SDL_KEYDOWN; break;
                    case SDLK_q: m->key[0x4] = event.type == SDL_KEYDOWN; break;
                    case SDLK_w: m->key[0x5] = event.type == SDL_KEYDOWN; break;
                    case SDLK_e: m->key[0x6] = event.type == SDL_KEYDOWN; break;
                    case SDLK_r: m->key[0xD] = event.type == SDL_KEYDOWN; break;
                    case SDLK_a: m->key[0x7] = event.type == SDL_KEYDOWN; break;
                    case SDLK_s: m->key[0x8] = event.type == SDL_KEYDOWN; break;
                    case SDLK_d: m->key[0x9] = event.type == SDL_KEYDOWN; break;
                    case SDLK_f: m->key[0xE] = event.type == SDL_KEYDOWN; break;
                    case SDLK_z: m->key[0xA] = event.type == SDL_KEYDOWN; break;
                    case SDLK_x: m->key[0x0] = event.type == SDL_KEYDOWN; break;
                    case SDLK_c: m->key[0xB] = event.type == SDL_KEYDOWN; break;
                    case SDLK_v: m->key[0xF] = event.type == SDL_KEYDOWN; break;
                }
                break;
        }
    }

    return 0;
}