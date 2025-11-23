#include "opcodes.h"

void x00E0(struct Chip8 *chip8)
{
        for (int i = 0; i < SCREEN_SIZE; i++)
                chip8->display[i] = 0;
        chip8->display_updated = true;
}
void x1NNN(struct Chip8 *chip8, uint16_t n) 
{
        chip8->pc = n & 0x0FFF;
}
void x00EE(struct Chip8 *chip8) 
{
        chip8->pc = chip8->stack[--chip8->sp];
        chip8->stack[chip8->sp] = 0;
}
void x2NNN(struct Chip8 *chip8, uint16_t n)
{
        chip8->stack[chip8->sp++] = chip8->pc; 
        chip8->pc = n & 0x0FFF;
}
void x3XNN(struct Chip8 *chip8, uint8_t x, uint8_t n) 
{
        if (chip8->V[x] == n)
                chip8->pc += 2;
}
void x4XNN(struct Chip8 *chip8,  uint8_t x, uint8_t n) 
{
        if (chip8->V[x] != n)
                chip8->pc += 2;
}
void x5XY0(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        if (chip8->V[x] == chip8->V[y])
                chip8->pc += 2;
}
void x9XY0(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        if (chip8->V[x] != chip8->V[y])
                chip8->pc += 2;
}
void x6XNN(struct Chip8 *chip8, uint8_t x, uint8_t n) 
{
        chip8->V[x] = n;
}
void x7XNN(struct Chip8 *chip8, uint8_t x, uint8_t n) 
{
        chip8->V[x] += n;
}
/* Arithmetic operators */
void x8XY0(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        chip8->V[x] = chip8->V[y];
}
void x8XY1(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        chip8->V[x] |= chip8->V[y];
        chip8->V[0xF] = 0;
}
void x8XY2(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        chip8->V[x] &= chip8->V[y];
        chip8->V[0xF] = 0;
}
void x8XY3(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        chip8->V[x] ^= chip8->V[y];
        chip8->V[0xF] = 0;
}
void x8XY4(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        uint16_t sum = chip8->V[x] + chip8->V[y];
        chip8->V[x] = sum & 0xFF;
        chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
}
void x8XY5(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        uint8_t flag = chip8->V[x] >= chip8->V[y] ? 1 : 0;
        chip8->V[x] = chip8->V[x] - chip8->V[y];
        chip8->V[0xF] = flag;
}
void x8XY7(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
        uint8_t flag = chip8->V[y] >= chip8->V[x] ? 1 : 0;
        chip8->V[x] = chip8->V[y] - chip8->V[x];
        chip8->V[0xF] = flag; 
}
void x8XY6(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
#if SHIFT_FROM_Y
        chip8->V[x] = chip8->V[y];
#endif
        uint8_t flag = chip8->V[x] & 0x1;
        chip8->V[x] = chip8->V[x] >> 1;
        chip8->V[0xF] = flag;
}
void x8XYE(struct Chip8 *chip8, uint8_t x, uint8_t y) 
{
#if SHIFT_FROM_Y 
        chip8->V[x] = chip8->V[y];
#endif
        uint8_t flag = (chip8->V[x] & 0x80) >> 7;
        chip8->V[x] = chip8->V[x] << 1;
        chip8->V[0xF] = flag;
}
void xANNN(struct Chip8 *chip8, uint16_t n) 
{
        chip8->I = n & 0x0FFF;
}
void xBNNN(struct Chip8 *chip8, uint16_t n)
{
        chip8->pc = n + chip8->V[0];
}
void xCXNN(struct Chip8 *chip8, uint8_t x, uint8_t n) 
{
        srand(time(NULL));
        chip8->V[x] = n & (rand() % (0xFF + 1));
}
void xDXYN(struct Chip8 *chip8, uint8_t x, uint8_t y, uint8_t n) 
{
        uint8_t x_pos = chip8->V[x] % SCREEN_WIDTH;
        uint8_t y_pos = chip8->V[y] % SCREEN_HEIGHT;
        chip8->V[0xF] = 0;
        for (int i = 0; i < n; i++) {
                if (y_pos + i >= 32)
                        break;
                uint8_t sprite_byte = chip8->memory[chip8->I + i];
                for (int j = 0; j < 8; j++) {
                        if (x_pos + j >= 64)
                                break;
                        uint8_t sprite_pixel = (sprite_byte >> (7 - j) & 1);
                        if (sprite_pixel) {
                                uint16_t screen_pos = (y_pos + i) * SCREEN_WIDTH + (x_pos + j); 
                                if (chip8->display[screen_pos]) {
                                        chip8->display[screen_pos] = 0;
                                        chip8->V[0xF] = 1;	
                                } else {
                                        chip8->display[screen_pos] = 1;
                                }
                        }
                }
        }
        chip8->display_updated = true;
}
void xEX9E(struct Chip8 *chip8, uint8_t x)
{
        if (chip8->keys_pressed[chip8->V[x]])
                chip8->pc += 2;
}
void xEXA1(struct Chip8 *chip8, uint8_t x)
{
        if (!chip8->keys_pressed[chip8->V[x]])
                chip8->pc += 2;
}
void xFX07(struct Chip8 *chip8, uint8_t x)
{
        chip8->V[x] = chip8->delay_timer;
}
void xFX15(struct Chip8 *chip8, uint8_t x)
{
        chip8->delay_timer = chip8->V[x];
}
void xFX18(struct Chip8 *chip8, uint8_t x)
{
        chip8->sound_timer = chip8->V[x];
}
void xFX1E(struct Chip8 *chip8, uint8_t x) 
{
        chip8->I += chip8->V[x];
}
void xFX0A(struct Chip8 *chip8, uint8_t x)
{
        for (uint8_t i = 0; i < 16; i++) {
                if (chip8->keys_pressed_released[i]) {
                        chip8->V[x] = i;
                        return;
                }
        }
        chip8->pc -= 2;
}
void xFX29(struct Chip8 *chip8, uint8_t x)
{
        uint8_t digit = chip8->V[x] & 0x0F;
        chip8->I = 0x50 + (digit * 5);
}
void xFX33(struct Chip8 *chip8, uint8_t x)
{
        chip8->memory[chip8->I] = chip8->V[x] / 100;
        chip8->memory[chip8->I + 1] = (chip8->V[x] % 100) / 10;
        chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
}
void xFX55(struct Chip8 *chip8, uint8_t x)
{
        for (uint8_t i = 0; i <= x; i++)
                chip8->memory[chip8->I + i] = chip8->V[i];
        chip8->I += x + 1;
}
void xFX65(struct Chip8 *chip8, uint8_t x)
{
        for (uint8_t i = 0; i <= x; i++)
                chip8->V[i] = chip8->memory[chip8->I + i];
        chip8->I += x + 1;
}
