#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_GAME_INIT_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_GAME_INIT_H
#include "game_api.h"
#include "game/Player.h"
#include "game/World.h"
void InitialPopulateChunk(Player& player, World& world, EngineApi& api);

void GameInitFn(GameMemory& memory, EngineApi& api);


#endif // DL_SDL3GPU_PROJECT_TEMPLATE_GAME_INIT_H
