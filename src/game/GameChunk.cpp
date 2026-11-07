#include "GameChunk.h"
#include "core/global_consts.h"
#include "game_api.h"

void GameChunk::Init(int cX, int cZ, float* noiseMap)
{
    x = cX;
    z = cZ;
    int maxHeight {150};
    int minHeight {50};
    int hDiff {maxHeight - minHeight};
    for (int bX {0}; bX < CHUNKX; ++bX)
    {
        for (int bZ {0}; bZ < CHUNKZ; ++bZ)
        {
            float noiseVal {noiseMap[bX * CHUNKZ + bZ]};
            int height {static_cast<int>(hDiff * noiseVal + minHeight)};
            for (int bY {0}; bY < CHUNKY; ++bY)
            {
                Block blk {};
                if (bY > height)
                    blk = BlockType::NOBLOCK;
                else if (bY == height)
                    blk = BlockType::GRASS;
                else
                    blk =BlockType::DIRT;
                blocks[bX][bY][bZ] = blk;
            }
        }
    }
}