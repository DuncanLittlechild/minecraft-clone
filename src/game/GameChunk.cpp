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
                Block blk {};
                if((bX == 0 || bX == CHUNKX - 1) && (bZ ==0 || bZ == CHUNKZ - 1))
                    blk = BlockType::STONE;
                else
                    blk =BlockType::DIRT;
                blocks[bX][bY][bZ] = blk;
            }
        }
    }
}