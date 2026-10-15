#include "Block.h"

SpriteRenderData Block::ToRenderData()
{
    SpriteRenderData rData{
        .x = x,
        .y = y,
        .z = z,
        .atlasHandle = type
    };

    return rData;
}