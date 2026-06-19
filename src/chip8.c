#include <SDL2/SDL.h>
#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

        // NNN ocupa los 12 bits bajos  →  máscara 0x0FFF
        // X   ocupa bits 8-11           →  máscara 0x0F00, luego >> 8
        // Y   ocupa bits 4-7            →  máscara 0x00F0, luego >> 4
        // N   ocupa bits 0-3            →  máscara 0x000F
        // NN  ocupa bits 0-7            →  máscara 0x00FF
    uint16_t opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
    // printf("PC: 0x%04X  Opcode: 0x%04X\n", chip8->pc, opcode);
    chip8->pc += 2; 
    switch(opcode & 0xF000){ //aplica máscara para quedarme con los 4 bits mas altos & es operador AND
        case (0x0000):
            switch (opcode & 0x00FF) {
                case 0x00E0: //00E0 — CLS. Limpia el display.
                    memset(chip8->display, 0, sizeof(chip8->display)); 
                    chip8->draw_flag = 1; //orden de pintar
                    break;
                case 0x00EE: //00EE — RET. Vuelve de subrutina: pc = stack[--sp].
                    chip8->pc = chip8->stack[--chip8->sp]; 
                    break;
            }
            break;
        case (0x1000): //1NNN — JP addr. Salta a NNN: pc = NNN.
            chip8->pc = opcode & 0x0FFF;
            break;
        case (0x2000): //2NNN — CALL addr. Llama a subrutina en NNN: guarda pc en stack, salta a NNN.
            chip8->stack[chip8->sp] = chip8->pc; 
            ++chip8->sp; 
            chip8->pc = opcode & 0x0FFF; 
            break;
        case(0x3000): //3XNN — SE Vx, byte. Salta la siguiente instrucción si Vx == NN.
            if(chip8->V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                chip8->pc+=2;
            }
            break;
        case(0x4000): //4XNN — SNE Vx, byte. Salta la siguiente instrucción si Vx != NN.
            if(chip8->V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                chip8->pc+=2;
            }
            break;
        case(0x5000): //5XY0 — SE Vx, Vy. Salta la siguiente instrucción si Vx == Vy.
            if(chip8->V[(opcode & 0x0F00) >> 8 ] == chip8->V[(opcode & 0x00F0) >> 4]) {
                chip8->pc+=2; 
            }
            break;
        case(0x6000): //6XNN — LD Vx, byte. Carga el valor NN en el registro Vx.
            chip8->V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;
        case(0x7000): //7XNN — ADD Vx, byte. Vx += NN (sin carry).
            chip8->V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF; 
            break;
        case(0x8000): //8XYN — operaciones entre registros. Nibble bajo N indica la operación.
            // X = (opcode & 0x0F00) >> 8  →  aísla el tercer nibble y desplaza 8 bits → índice de Vx
            // Y = (opcode & 0x00F0) >> 4  →  aísla el segundo nibble y desplaza 4 bits → índice de Vy
            switch(opcode & 0x000F){
                case (0x0): //8XY0 — LD Vx, Vy. Vx = Vy.
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case (0x1): //8XY1 - OR Vx, VY
                    chip8->V[(opcode & 0x0F00) >> 8] |= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
            
                case (0x2): //8XY2 - AND Vx, VY
                    chip8->V[(opcode & 0x0F00) >> 8] &= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case (0x3): //8XY3 - XOR Vx, Vy
                    chip8->V[(opcode & 0x0F00) >> 8] ^= chip8->V[(opcode & 0x00F0) >> 4];
                    break;
                case (0x4): { //8XY4 - Vx += Vy
                    uint16_t result = chip8->V[(opcode & 0x0F00) >> 8] + chip8->V[(opcode & 0x00F0) >> 4];
                    chip8->V[0xF] = result > 255 ? 1 : 0;
                    chip8->V[(opcode & 0x0F00) >> 8] = result & 0xFF;
                    break;
                }
                case (0x5):{ //SUB Vx, Vy | Vx -= Vy, VF = !borrow
                    if(chip8->V[(opcode & 0x0F00) >> 8] >= chip8->V[(opcode & 0x00F0) >> 4]){
                        //V[F] = 1 (no hay borrow)
                        chip8->V[0xF] = 1;
                    }
                    else if(chip8->V[(opcode & 0x0F00) >> 8] < chip8->V[(opcode & 0x00F0) >> 4]) {
                        //V[F] = 0 (hay borrow)
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[(opcode & 0x0F00) >> 8] -= chip8->V[(opcode & 0x00F0) >> 4];
                    break; 
                }
                case (0x6): //SHR Vx | VF = Vx & 1, Vx >>= 1
                    chip8->V[0xF] = chip8->V[(opcode & 0x0F00) >> 8]  & 1; 
                    chip8->V[(opcode & 0x0F00) >> 8] >>=1; 
                    break; 
                case (0x7): //SUBN Vx, Vy | Vx = Vy - Vx, VF = !borrow
                    chip8->V[0xF] = chip8->V[(opcode & 0x00F0) >> 4] >= chip8->V[(opcode & 0x0F00) >> 8] ? 1: 0;
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->V[(opcode & 0x00F0) >> 4] - chip8->V[(opcode & 0x0F00) >> 8];
                    break; 
                case (0x8): //SHL Vx | VF = Vx >> 7, Vx <<= 1
                    chip8->V[0xF] = chip8->V[(opcode & 0x0F00) >> 8]  >> 7; 
                    chip8->V[(opcode & 0x0F00) >> 8] <<=1; 
                    break; 
            }

            break;
        case(0x9000): //9XY0 — SNE Vx, Vy. Salta la siguiente instrucción si Vx != Vy.
            if(chip8->V[(opcode & 0x0F00) >> 8 ] != chip8->V[(opcode & 0x00F0) >> 4]) {
                chip8->pc+=2; 
            }
            break;
        case(0xA000): //ANNN — LD I, addr. I = NNN.
            chip8->I = opcode & 0x0FFF; 
            break;
        case(0xB000): //BNNN — JP V0, addr. pc = NNN + V0.
            chip8->pc = (opcode & 0x0FFF) + chip8->V[0];
            break;
        case(0xC000): //CXNN — RND Vx, byte. Vx = rand() & NN.
            chip8 -> V[(opcode & 0x0F00) >> 8] = rand() & (opcode & 0x00FF); 
            break;
        case(0xD000): { //DXYN — DRAW Vx, Vy, nibble. Dibuja sprite N bytes desde memory[I] en (Vx,Vy). VF=colisión.
                // 1. Extrae X, Y, N del opcode
                // 2. Vx = V[X], Vy = V[Y] — posición en pantalla
                // 3. VF = 0 — reset del flag de colisión
                // 4. Para cada fila row de 0 a N:
                //     - Lee el byte sprite = memory[I + row]
                //     - Para cada bit col de 0 a 7:
                //         - Si el bit está activo (sprite & (0x80 >> col))
                //     - Calcula el pixel en el display: pos = (Vy + row) * 64 + (Vx + col)
                //     - Si ese pixel ya estaba activo → colisión: VF = 1
                //     - XOR el pixel: display[pos] ^= 1
                // 5. Activa draw_flag
                uint8_t x = (opcode & 0x0F00) >> 8;
                uint8_t y = (opcode & 0x00F0) >> 4;
                uint8_t n = (opcode & 0x00F);
                uint8_t posX = chip8->V[x];
                uint8_t posY = chip8->V[y];
                chip8->V[0xF] = 0; // reset flag colision
                for(int row = 0; row < n; row++){
                    uint8_t byteSpray = chip8->memory[chip8->I+row]; 
                    for(int col = 0; col<8; col++) {
                        if (byteSpray & (0x80 >> col)) {
                            uint16_t pxPos = ((posY + row) % 32) * 64 + ((posX + col) % 64); //la pantallas es 64x32 = 2048px
                            //cualquier pixel fila 4 o mas desbordaría con uint8_t, es fácilmente visible si lo cambio a 8 bits
                            if(chip8->display[pxPos]){
                                chip8->V[0xF] = 1; 

                            }
                            chip8->display[pxPos] ^=1;
                            chip8->draw_flag = 1; 
                        } 
                    }
                }
                break;
        }


        case(0xE000): //EX9E/EXA1 — SKP/SKNP. Salta según si tecla Vx está pulsada o no.
            break;
        case(0xF000):
            switch (opcode & 0x00FF){
                case (0x07): //FX07 — LD Vx, DT. Vx = delay_timer.
                    chip8->V[(opcode & 0x0F00) >> 8] = chip8->delay_timer; 
                    break;
                case(0x15): //FX15 — LD DT, Vx. delay_timer = Vx.
                    chip8->delay_timer = chip8->V[(opcode & 0x0F00) >> 8]; 
                    break;
                case(0x18): //FX18 — LD ST, Vx. sound_timer = Vx.
                    chip8->sound_timer = chip8->V[(opcode & 0x0F00) >> 8]; 
                    break;
                case(0x1E): //FX1E — ADD I, Vx. I += Vx.
                    chip8->I+=chip8->V[(opcode & 0x0F00) >> 8]; 
                    break;
                case(0x29): //FX29 — LD F, Vx. I apunta al sprite del dígito Vx en el fontset.
                    chip8->I = chip8->V[(opcode & 0x0F00) >> 8] * 5;  // el sprite del dígito V[X] está en la dirección V[X] * 5 de memoria
                    //Luego cuando DXYN (draw) use I, irá a buscar los bytes del sprite allí
                    break;
                case(0x33): //FX33 — LD B, Vx. BCD de Vx en memory[I], memory[I+1], memory[I+2].
                    chip8->memory[chip8->I] = chip8->V[(opcode & 0x0F00) >> 8] / 100; //centenas
                    chip8->memory[chip8->I+1] = (chip8->V[(opcode & 0x0F00) >> 8] / 10) % 10; //decenas 
                    chip8->memory[chip8->I+2] = chip8->V[(opcode & 0x0F00) >> 8] % 10; //unidades
                    break;
                case(0x55): //FX55 — LD [I], Vx. Vuelca V0-Vx en memoria desde I.
                    for(unsigned int i = 0; i<=chip8->V[(opcode & 0x0F00) >> 8]; i++){
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                case(0x65): //FX65 — LD Vx, [I]. Carga V0-Vx desde memoria en I.
                    for(unsigned int i = 0; i<=chip8->V[(opcode & 0x0F00) >> 8]; i++){
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
            }
        break;
        default:
            fprintf(stderr, "Unknown code: 0x%04X\n", opcode); break; 

    }
}