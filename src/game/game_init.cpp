#include "game_init.h"
#include "World.h"
#include "core/global_consts.h"
#include "game/GameData.h"
void InitialPopulateChunk(Player& player, World& world, EngineApi& api)
{
    static int chunkOffsetFactorX {9 * CHUNKX};
    static int chunkOffsetFactorZ {9 * CHUNKZ};
    int playerX {static_cast<int>(player.pos.x)};
    int playerZ {static_cast<int>(player.pos.z)};

    // Get all coordinates in a 9x9 grid with the centre on the player
    int xStart {playerX - chunkOffsetFactorX};
    int xEnd {playerX + chunkOffsetFactorX};
    int zStart {playerZ - chunkOffsetFactorZ};
    int zEnd {playerZ + chunkOffsetFactorZ};

    for (int x {xStart}; x <= xEnd; x += CHUNKX)
    {
        for (int z {zStart}; z <= zEnd; z += CHUNKZ)
        {
            world.chunkSpawnQueue.emplace_back(x, z);
        }
    }
}

void GameInitFn(GameMemory& memory, EngineApi& api)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    new (data) GameData{};
    data->player.pos = {8.0f, 158.f, 8.0f};
    data->player.target = {9.0f, 158.f, 9.0f};
    data->player.speed = 2.0f;
    data->player.rSpeed = 2.0f;
    data->player.Init(api);
    data->world.Init(5);
    InitialPopulateChunk(data->player, data->world, api);
};

