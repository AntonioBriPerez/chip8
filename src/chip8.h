#ifndef CHIP8_H 
#define CHIP8_H
#include <stdint.h>

typedef struct {    
    uint8_t memory[4096];       // 4K memory
    uint8_t V[16];              // 16 registers
    uint16_t I;             // Index register
    uint16_t pc;       // Program counter
    uint16_t stack[16];     // Stack for subroutine calls
    uint8_t sp;             // Stack pointer
    uint8_t delay_timer; // cuenta atras a cero a 60Hz, 
    uint8_t sound_timer; // cuenta atras a cero a 60Hz, emite un pitido mientras es mayor que cero
    uint8_t keypad[16];  // estado de las 16 teclas 

    uint8_t display[64 * 32]; //framebuffer, 1 byte por pixel
    uint8_t draw_flag; //indica si se debe redibujar la pantalla
} Chip8;

void chip8_init(Chip8 *chip8);
void chip8_load_rom(Chip8 *chip8, const char *filename);
void chip8_cycle(Chip8 *chip8);
#endif