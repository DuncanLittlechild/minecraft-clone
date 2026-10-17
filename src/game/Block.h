#ifndef DL_TEMPLATE_BLOCK_H
#define DL_TEMPLATE_BLOCK_H
#include "game_api.h"

struct Block {
    SpriteRenderData ToRenderData();

    // Enums must match the texture atlas. 
    // If you count up to the number the enum represents from left to right on the atlas,
    // wrapping at the end of lines, you should get the correct texture.
    BlockType::Type type{};

    // Block coordinates within its chunk
    int x, y, z;
};

#endif