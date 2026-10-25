#include "GameChunk.h"
#include "core/global_consts.h"

void GameChunk::Init(int cX, int cZ, EngineApi& api)
{
    x = cX;
    z = cZ;
    for (int bX {0}; bX < CHUNKX; ++bX)
    {
        for (int bY {0}; bY < CHUNKY; ++bY)
        {
            for (int bZ {0}; bZ < CHUNKZ; ++bZ)
            {
                blocks[bX][bY][bZ] = BlockType::DIRT;
            }
        }
    }
}