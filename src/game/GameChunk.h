#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H

#include "core/global_consts.h"
#include "game_api.h"
struct GameChunk {
    BlockType::Type blocks[CHUNKX][CHUNKY][CHUNKZ];
    int handle;
    int x, y;
    bool dirty{false};

    void Init(int, int, EngineApi&);
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
