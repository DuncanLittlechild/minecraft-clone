#include "SDL3/SDL.h"
#include "stb_image.h"
#include "stb_utils.h"

SDL_Surface* STB_Utils::LoadImage(const char* imgName, int desiredChannels)
{
    int width {0}, height {0}, channels {0};
    stbi_uc* textureAtlasBmp {stbi_load(imgName, &width, &height, &channels, 4)};
    if (!textureAtlasBmp)
    {
        printf("Load failed: %s\n", stbi_failure_reason());
        perror("Error");
    }

    return SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, textureAtlasBmp, width * 4);
}
