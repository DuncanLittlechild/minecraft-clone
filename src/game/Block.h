#ifndef DL_TEMPLATE_BLOCK_H
#define DL_TEMPLATE_BLOCK_H
#include "game_api.h"

constexpr float BLOCKTEXSIZE {16/256};
struct Block {
    SpriteRenderData ToRenderData()
    {
        SpriteRenderData rData{
            .x = x,
            .y = y,
            .z = z,
        };

        return rData;
    }

    // Enums must match the texture atlas. 
    // If you count up to the number the enum represents from left to right on the atlas,
    // wrapping at the end of lines, you should get the correct texture.
    enum Type {
        DIRT,
    } type{};

    float x, y, z;
};

#endif