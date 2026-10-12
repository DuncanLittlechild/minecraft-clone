#ifndef DL_SDLTEMPLATE_HELPERS_H
#define DL_SDLTEMPLATE_HELPERS_H
#include "SDL3/SDL.h"
namespace STB_Utils{
    SDL_Surface* LoadImage(const char* imgName, int desiredChannels);
}


#endif