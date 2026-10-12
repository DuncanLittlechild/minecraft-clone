#include "Block.h"
#include <array>

// Indexing into this array will give one the numbers to multiply blockinc by to find the starting {x,y} coords
// of the texture in the texture atlas
constexpr std::array<std::pair<int, int>, Block::TYPE_COUNT> 
BLOCKCOORDLOOKUP {{
    {0, 0}
}};

SpriteRenderData Block::ToRenderData()
{
    SpriteRenderData rData{
        .x = x,
        .y = y,
        .z = z,
        .u0 = BLOCKCOORDLOOKUP[type].first,
        .v0 = BLOCKCOORDLOOKUP[type].second
    };

    return rData;
}