#include "GameChunk.h"
#include "core/global_consts.h"
#include "game_api.h"


void GameChunk::Init(int cX, int cY)
{
    // TODO: link handle to value retreived from the renderer
    handle = 0;
    x = cX;
    y = cY;
    for (int x {0}; x < CHUNKWIDTH; ++x)
    {
        for (int y {0}; y < CHUNKHEIGHT; ++y)
        {
            for (int z {0}; z < CHUNKDEPTH; ++z)
            {
                if(z == 0)
                {
                    blocks[x][y][z] = BlockType::DIRT;
                }
                else
                {
                    blocks[x][y][z] = BlockType::NOBLOCK;
                }
            }
        }
    }
}