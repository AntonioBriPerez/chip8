#include "display.h"
#include <SDL2/SDL.h>


void display_render(SDL_Renderer *render, uint8_t display[64 * 32]) {
    for(unsigned int i=0; i<64*32; i++){
        if(display[i] == 1){
            int X = i % 64; 
            int Y = i / 64; 
            SDL_SetRenderDrawColor(render, 255,255,255,255);
            SDL_Rect rect = {X * 10, Y * 10, 10, 10};
            SDL_RenderFillRect(render, &rect);
        }
    }
}