#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdio.h>
#include "sound.h"

#define DEBUG 0
#define SHIFT_FROM_Y 1
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

struct Chip8 {
        uint16_t pc;
        uint16_t I;
        uint16_t sp;

        uint8_t memory[4096];
        uint16_t stack[16];

        uint8_t delay_timer;
        uint8_t sound_timer;

        uint8_t V[16];

        uint8_t display[SCREEN_SIZE];
        bool display_updated;

        uint8_t keys_pressed[16];
        uint8_t keys_pressed_released[16];
};
static const struct {
        SDL_Scancode scancode;
        uint8_t key;
} keymap[] = {
        {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3}, {SDL_SCANCODE_4, 0xC},
        {SDL_SCANCODE_Q, 0x4}, {SDL_SCANCODE_W, 0x5}, {SDL_SCANCODE_E, 0x6}, {SDL_SCANCODE_R, 0xD},
        {SDL_SCANCODE_A, 0x7}, {SDL_SCANCODE_S, 0x8}, {SDL_SCANCODE_D, 0x9}, {SDL_SCANCODE_F, 0xE},
        {SDL_SCANCODE_Z, 0xA}, {SDL_SCANCODE_X, 0x0}, {SDL_SCANCODE_C, 0xB}, {SDL_SCANCODE_V, 0xF},
};

void read_rom(struct Chip8 *chip8, char *filename);
uint16_t fetch_opcodes(struct Chip8 *chip8);
void fetch_decode_exec(struct Chip8 *chip8);
void update_timers(struct Chip8 *chip8, struct AudioToneGenerator *tone_gen);
void clear_input(struct Chip8 *chip8);

#endif // !CHIP8_H
