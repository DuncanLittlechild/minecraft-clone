#include "game_init.h"
#include "World.h"
#include "game/GameData.h"
void InitialPopulateChunk(Player& player, World& world, EngineApi& api)
{
    int playerX {static_cast<int>(player.pos.x)};
    int playerZ {static_cast<int>(player.pos.z)};

    // Get all coordinates in a 9x9 grid with the centre on the player
    int xStart {playerX - 9};
    int xEnd {playerX + 9};
    int zStart {playerZ - 9};
    int zEnd {playerZ + 9};

    for (int x {xStart}; x < xEnd; ++x)
    {
        for (int z {zStart}; z < zEnd; ++z)
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

