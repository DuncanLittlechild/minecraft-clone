#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
#include "game_api.h"

enum Biome {
    GRASSLAND_BIOME,
    BIOME_COUNT,
    MIN_BIOME = 0
};

struct GameChunk {
    BlockType::Type blocks[CHUNKX][CHUNKY][CHUNKZ];
    int handle;
    int x, z;
    bool dirty{false};
    Biome biome{};

    void Init(int, int, float*);
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_GAMECHUNK_H
