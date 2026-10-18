#include "GameChunk.h"
#include "core/global_consts.h"
#include "game_api.h"


void GameChunk::Init(int cX, int cY, EngineApi& api)
{
    x = cX;
    y = cY;
    for (int x {0}; x < CHUNKX; ++x)
    {
        for (int y {0}; y < CHUNKY; ++y)
        {
            for (int z {0}; z < CHUNKZ; ++z)
            {
                blocks[x][y][z] = BlockType::DIRT;
            }
        }
    }

    handle = api.GetChunkHandle(blocks, x, y);
}