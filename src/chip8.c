#include "chip8.h"
#include "sound.h"
#include "opcodes.h"

void read_rom(struct Chip8 *chip8, char *filename)
{
        FILE *fptr = fopen(filename, "rb");
        if (!fptr) {
                fprintf(stderr, "Error opening file\n");
                exit(1);
        }
        int byte;
        int i = chip8->pc;
        while ((byte = fgetc(fptr)) != EOF) {
                chip8->memory[i] = (uint8_t)byte;
                i++;
        }
        fclose(fptr);
#if DEBUG
        printf("Opcodes:\n");
        for (int j = 0x200; j < i; j += 2) {
                uint16_t opcode = chip8->memory[j] << 8 | chip8->memory[j + 1];
                printf("0x%03X: 0x%04X\n", j, opcode);
        }
#endif
}
uint16_t fetch_opcodes(struct Chip8 *chip8)
{
        uint8_t hi = chip8->memory[chip8->pc];
        uint8_t lo = chip8->memory[chip8->pc + 1];
#if DEBUG
        if (((hi << 8) | lo))
                printf("pc: %04X points to %04X\n", chip8->pc,(hi << 8) | lo);
#endif
        chip8->pc += 2;
        return (hi << 8) | lo;
}
void fetch_decode_exec(struct Chip8 *chip8)
{
        uint16_t op = fetch_opcodes(chip8);
        switch (op & 0xF000) {
                case 0x0000:
                        switch (op) {
                                case 0x00EE:
                                        x00EE(chip8);
                                        break;
                                case 0x00E0:
                                        x00E0(chip8);
                                        break;
                        }
                        break;
                case 0x1000: 
                        x1NNN(chip8, op & 0x0FFF);
                        break;
                case 0x2000: 
                        x2NNN(chip8, op & 0x0FFF);
                        break;
                case 0x3000:
                        x3XNN(chip8, (op & 0x0F00) >> 8, op & 0x00FF);
                        break;
                case 0x4000:
                        x4XNN(chip8, (op & 0x0F00) >> 8, op & 0x00FF);
                        break;
                case 0x5000:
                        x5XY0(chip8, (op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
                        break;
                case 0x6000: 
                        x6XNN(chip8, (op & 0x0F00) >> 8, op & 0x00FF);
                        break;
                case 0x7000: 
                        x7XNN(chip8, (op & 0x0F00) >> 8, op & 0x00FF);
                        break;
                case 0x8000:
                        ;
                        uint8_t x = (op & 0x0F00) >> 8;
                        uint8_t y = (op & 0x00F0) >> 4;
                        switch (op & 0x000F) {
                                case 0x0:
                                        x8XY0(chip8, x, y);
                                        break;
                                case 0x1:
                                        x8XY1(chip8, x, y);
                                        break;
                                case 0x2:
                                        x8XY2(chip8, x, y);
                                        break;
                                case 0x3:
                                        x8XY3(chip8, x, y);
                                        break;
                                case 0x4:
                                        x8XY4(chip8, x, y);
                                        break;
                                case 0x5:
                                        x8XY5(chip8, x, y);
                                        break;
                                case 0x7:
                                        x8XY7(chip8, x, y);
                                        break;
                                case 0x6:
                                        x8XY6(chip8, x, y);
                                        break;
                                case 0xE:
                                        x8XYE(chip8, x, y);
                                        break;
                        }
                        break;
                case 0x9000:
                        x9XY0(chip8, (op & 0x0F00) >> 8, (op & 0x00F0) >> 4);
                        break;
                case 0xA000: 
                        xANNN(chip8, op & 0x0FFF);
                        break;
                case 0xB000: 
                        xBNNN(chip8, op & 0x0FFF);
                        break;
                case 0xC000:
                        xCXNN(chip8, (op & 0x0F00) >> 8, op & 0x00FF);
                        break;
                case 0xE000:
                        switch (op & 0x00FF) {
                                case 0x009E:
                                        xEX9E(chip8, (op & 0x0F00) >> 8);
                                        break;
                                case 0x00A1:
                                        xEXA1(chip8, (op & 0x0F00) >> 8);
                                        break;
                        }
                        break;
                case 0xD000: 
                        xDXYN(chip8, (op & 0x0F00) >> 8, (op & 0x00F0) >> 4, op & 0x000F);
                        break;
                case 0xF000:
                        x = (op & 0x0F00) >> 8;
                        switch (op & 0x00FF) {
                                case 0x07:
                                        xFX07(chip8, x);
                                        break;
                                case 0x15:
                                        xFX15(chip8, x);
                                        break;
                                case 0x18:
                                        xFX18(chip8, x);
                                        break;
                                case 0x1E:
                                        xFX1E(chip8, x);
                                        break;
                                case 0x0A:
                                        xFX0A(chip8, x);
                                        break;
                                case 0x29:
                                        xFX29(chip8, x);
                                        break;
                                case 0x33:
                                        xFX33(chip8, x);
                                        break;
                                case 0x55:
                                        xFX55(chip8, x);
                                        break;
                                case 0x65:
                                        xFX65(chip8, x);
                                        break;
                        }
                        break;
                default:
                        printf("Unknown opcode: %04X\n", op);
                        break;
        }
}
void update_timers(struct Chip8 *chip8, struct AudioToneGenerator *tone_gen)
{
        if (chip8->delay_timer > 0)
                chip8->delay_timer--;
        if (chip8->sound_timer > 0) {
                /* Beep */
                audio_tone_start(tone_gen);
                chip8->sound_timer--;
        } else {
                audio_tone_stop(tone_gen);
                /* Stop beeping */
        }
}
void clear_input(struct Chip8 *chip8)
{
        for (int i = 0; i <  16; i++)
                chip8->keys_pressed_released[i] = 0;
}
