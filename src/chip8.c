#include <SDL2/SDL.h>
#include "chip8.h"
#include <stdio.h>
#include <string.h>
static const uint8_t fontset[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

void chip8_init(Chip8 *chip8) {
    memset(chip8, 0, sizeof(Chip8)); // Limpia toda la estructura, incluyendo memoria, registros, etc.
    chip8->pc = 0x200; 
    memcpy(chip8->memory, fontset, sizeof(fontset)); // Carga el fontset en la memoria
}

void chip8_load_rom(Chip8 *chip8, const char *filename) {

    if (filename == NULL) {
        fprintf(stderr, "Error: No se proporcionó un archivo ROM.\n");
        return;
    }
    FILE *file = fopen(filename, "rb");
    //comprobamos si fdevuelve NULL
    if (file == NULL) {
        fprintf(stderr, "File returned NULL");
        return; 
    }

    //cargamos la ROM en toda la memoria reservada pa chip8
    fread(&chip8->memory[0x200], 1, sizeof(chip8->memory) - 0x200, file);
    fclose(file);
}

void chip8_cycle(Chip8 *chip8){
    //fetch

    //opcode son 2 bytes, leemos dos celdas, el << 8 es para desplazar el byte alto (es Big Endian) (0x12) 8 posiciones a la izq
    // luego combinamos con el byte bajo (0x00) usando OR: 0x1200 | 0x00  →  0x1200
    uint16_t opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
    chip8->pc += 2; 
    switch(opcode & 0xF000){ //aplica máscara para quedarme con los 4 bits mas altos & es operador AND
        case (0X0000):
            break; 
        case (0x00E0):
            break;
        case(0x00EE):
            break;
        default:
            fprintf(stderr, "Unknown code: 0x%04X\n", opcode); break; 

    }
}