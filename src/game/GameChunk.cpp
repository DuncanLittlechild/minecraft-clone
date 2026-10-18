#include "GameChunk.h"
#include "core/global_consts.h"
#include "game_api.h"


void GameChunk::Init(int cX, int cY)
{
    // TODO: link handle to value retreived from the renderer
    handle = 0;
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
}