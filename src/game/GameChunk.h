#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H

#include "core/global_consts.h"
#include "game_api.h"
struct GameChunk {
    BlockType::Type blocks[CHUNKWIDTH][CHUNKHEIGHT][CHUNKDEPTH];
    int handle;
    int x, y;

    void Init(int, int);
};


#endif // DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
