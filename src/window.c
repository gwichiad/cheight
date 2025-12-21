#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "chip8.h"
#include "sound.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_AudioStream *stream = NULL;

struct Appstate {
        struct Chip8 *chip8;
        struct AudioToneGenerator *tone_gen;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
        SDL_SetAppMetadata("Chipie", "1.0", "xyz.gwichiad.chipie");
        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        if (!SDL_CreateWindowAndRenderer("Chipie", 
                                640, 320, SDL_WINDOW_RESIZABLE, 
                                &window, &renderer)) {
                SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
                SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        SDL_SetRenderLogicalPresentation(renderer, 64, 32, 
                        SDL_LOGICAL_PRESENTATION_LETTERBOX);
        struct Appstate *state = SDL_malloc(sizeof(struct Appstate));
        SDL_AudioSpec spec = {
                .format = SDL_AUDIO_F32,  /* float samples */
                .channels = 1,             /* mono */
                .freq = 8000              /* 8000 Hz sample rate */
        };
        stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 
                        &spec, NULL, NULL);
        if (!stream) {
                SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }
        SDL_ResumeAudioStreamDevice(stream);
        state->tone_gen = SDL_malloc(sizeof(struct AudioToneGenerator));
        audio_tone_init(state->tone_gen, stream, 330, 8000);

        state->chip8 = SDL_malloc(sizeof(struct Chip8));
        SDL_memset(state->chip8, 0, sizeof(struct Chip8));
        state->chip8->pc = 0x200;
        state->chip8->display_updated = true;

	init_font(state->chip8);

        if (argv[1]) {
                read_rom(state->chip8, argv[1]);
        } else { 
                fprintf(stderr, "No file specified\n");
                printf("Usage: chipie FILENAME\n");
                return SDL_APP_FAILURE;
        }
        appstate[0] = state;
        return SDL_APP_CONTINUE;
}
SDL_AppResult handle_input(struct Chip8 *chip8, SDL_Scancode key, bool pressed)
{
        for (int i = 0; i < sizeof(keymap) / sizeof(keymap[0]); i++) {
                if (keymap[i].scancode == key) {
                        if (pressed) {
                                chip8->keys_pressed[keymap[i].key] = 1;
                        } else {
                                chip8->keys_pressed_released[keymap[i].key] = 1;
                                chip8->keys_pressed[keymap[i].key] = 0;
                        }
                        break;
                }
        }
        return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
        static bool key_pressed[SDL_SCANCODE_COUNT] = {false}; 
        struct Appstate *state = appstate;
        switch (event->type) {
                case SDL_EVENT_QUIT:
                        return SDL_APP_SUCCESS;
                case SDL_EVENT_KEY_DOWN:
                        if (!event->key.repeat) {
                                handle_input(state->chip8, event->key.scancode, true);
                                key_pressed[event->key.scancode] = true;
                        }
                        break;
                case SDL_EVENT_KEY_UP:
                        if (key_pressed[event->key.scancode]) {
                                handle_input(state->chip8, event->key.scancode, false);
                                key_pressed[event->key.scancode] = false;
                        }
                        break;
        }
        return SDL_APP_CONTINUE;
}
SDL_AppResult SDL_AppIterate(void *appstate)
{
        static double last_exec_time = 0.0;
        static double last_delay_time = 0.0;
        const double exec_interval = 1.0 / 700.0;
        const double delay_interval = 1.0 / 60.0;
        const double now_sec = ((double)SDL_GetTicks()) / 1000.0;
        struct Appstate *state = appstate;

        if (now_sec - last_exec_time >= exec_interval) {
                fetch_decode_exec(state->chip8);
                clear_input(state->chip8);
                last_exec_time = now_sec;
        }
        if (now_sec - last_delay_time >= delay_interval) {
                update_timers(state->chip8, state->tone_gen);
                last_delay_time = now_sec;
        }
        if (state->chip8->display_updated) {
                SDL_RenderClear(renderer);
                for (int i = 0; i < SCREEN_SIZE; i++) {
                        if (state->chip8->display[i])
                                SDL_SetRenderDrawColor(renderer, 255, 255, 255,
                                                SDL_ALPHA_OPAQUE);
                        else 
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0,
                                                SDL_ALPHA_OPAQUE);
                        SDL_RenderPoint(renderer, 
                                        i % SCREEN_WIDTH, i / SCREEN_WIDTH);
                }
                state->chip8->display_updated = false;
                SDL_RenderPresent(renderer);
        }
        audio_tone_update(state->tone_gen);

        SDL_Delay(exec_interval * 1000);

        return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
        struct Appstate *state = appstate;
        if (stream)
                SDL_DestroyAudioStream(stream);
        SDL_free(state->chip8);
        SDL_free(state->tone_gen);
        SDL_free(state);
}
