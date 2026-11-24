#include "GameChunk.h"
#include "core/dl_rand.hpp"
#include "core/global_consts.h"
#include "game_api.h"
// Tree generation rules
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
            bool hasTree {DL_Rand::GetIntInRange(1, 20) == 1};
            float noiseVal {noiseMap[bX * CHUNKZ + bZ]};
            int height {static_cast<int>(hDiff * noiseVal + minHeight)};
            int treeHeight;
            if(hasTree)
            {
                treeHeight = DL_Rand::GetIntInRange(3, 6) + height;
            }
            else
            {
                treeHeight = height;
            }
            for (int bY {0}; bY < CHUNKY; ++bY)
            {
                Block blk {};

                if (bY > treeHeight)
                    blk = BlockType::NOBLOCK;
                else if (bY > height)
                    blk = BlockType::LOG;
                else if (bY == height)
                    blk = BlockType::GRASS;
                else
                    blk =BlockType::DIRT;
                blocks[bX][bY][bZ] = blk;
            }
        }
    }
}