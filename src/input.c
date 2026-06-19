#include "input.h"
#include <SDL2/SDL.h>

void read_events(SDL_Event *event, uint8_t keypad[16]) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_1: keypad[0x1] = 1; break;
            case SDLK_2: keypad[0x2] = 1; break;
            case SDLK_3: keypad[0x3] = 1; break;
            case SDLK_4: keypad[0xC] = 1; break;
            case SDLK_q: keypad[0x4] = 1; break;
            case SDLK_w: keypad[0x5] = 1; break;
            case SDLK_e: keypad[0x6] = 1; break;
            case SDLK_r: keypad[0xD] = 1; break;
            case SDLK_a: keypad[0x7] = 1; break;
            case SDLK_s: keypad[0x8] = 1; break;
            case SDLK_d: keypad[0x9] = 1; break;
            case SDLK_f: keypad[0xE] = 1; break;
            case SDLK_z: keypad[0xA] = 1; break;
            case SDLK_x: keypad[0x0] = 1; break;
            case SDLK_c: keypad[0xB] = 1; break;
            case SDLK_v: keypad[0xF] = 1; break;
        }
    }
    if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
            case SDLK_1: keypad[0x1] = 0; break;
            case SDLK_2: keypad[0x2] = 0; break;
            case SDLK_3: keypad[0x3] = 0; break;
            case SDLK_4: keypad[0xC] = 0; break;
            case SDLK_q: keypad[0x4] = 0; break;
            case SDLK_w: keypad[0x5] = 0; break;
            case SDLK_e: keypad[0x6] = 0; break;
            case SDLK_r: keypad[0xD] = 0; break;
            case SDLK_a: keypad[0x7] = 0; break;
            case SDLK_s: keypad[0x8] = 0; break;
            case SDLK_d: keypad[0x9] = 0; break;
            case SDLK_f: keypad[0xE] = 0; break;
            case SDLK_z: keypad[0xA] = 0; break;
            case SDLK_x: keypad[0x0] = 0; break;
            case SDLK_c: keypad[0xB] = 0; break;
            case SDLK_v: keypad[0xF] = 0; break;
        }
    }
}
