#ifndef INPUT_H
#define INPUT_H
#include <stdint.h>
#include <SDL2/SDL.h>

void read_events(SDL_Event*, uint8_t[16]); 

#endif