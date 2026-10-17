#include "Block.h"

SpriteRenderData Block::ToRenderData()
{
    SpriteRenderData rData{
        .x = static_cast<float>(x),
        .y = static_cast<float>(y),
        .z = static_cast<float>(z),
        .atlasHandle = type
    };

    return rData;
}