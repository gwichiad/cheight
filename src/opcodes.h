#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"
#include <stdlib.h>
#include <time.h>

void x00E0(struct Chip8 *chip8);
void x1NNN(struct Chip8 *chip8, uint16_t n);
void x00EE(struct Chip8 *chip8);
void x2NNN(struct Chip8 *chip8, uint16_t n);
void x3XNN(struct Chip8 *chip8, uint8_t x, uint8_t n);
void x4XNN(struct Chip8 *chip8, uint8_t x, uint8_t n);
void x5XY0(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x9XY0(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x6XNN(struct Chip8 *chip8, uint8_t x, uint8_t n);
void x7XNN(struct Chip8 *chip8, uint8_t x, uint8_t n);
void x8XY0(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY1(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY2(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY3(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY4(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY5(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY7(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XY6(struct Chip8 *chip8, uint8_t x, uint8_t y);
void x8XYE(struct Chip8 *chip8, uint8_t x, uint8_t y);
void xANNN(struct Chip8 *chip8, uint16_t n);
void xBNNN(struct Chip8 *chip8, uint16_t n);
void xCXNN(struct Chip8 *chip8, uint8_t x, uint8_t n);
void xDXYN(struct Chip8 *chip8, uint8_t x, uint8_t y, uint8_t n);
void xEX9E(struct Chip8 *chip8, uint8_t x);
void xEXA1(struct Chip8 *chip8, uint8_t x);
void xFX07(struct Chip8 *chip8, uint8_t x);
void xFX15(struct Chip8 *chip8, uint8_t x);
void xFX18(struct Chip8 *chip8, uint8_t x);
void xFX1E(struct Chip8 *chip8, uint8_t x);
void xFX0A(struct Chip8 *chip8, uint8_t x);
void xFX29(struct Chip8 *chip8, uint8_t x);
void xFX33(struct Chip8 *chip8, uint8_t x);
void xFX55(struct Chip8 *chip8, uint8_t x);
void xFX65(struct Chip8 *chip8, uint8_t x);

#endif // !OPCODES_H
