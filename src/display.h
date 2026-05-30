#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <SDL2/SDL.h>

void display_render(SDL_Renderer*, uint8_t display[64 * 32]);

#endif