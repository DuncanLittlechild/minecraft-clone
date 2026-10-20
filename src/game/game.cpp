#include "GameData.h"
#include "game_api.h"
#include "imgui/dl_imgui_frontend.h"
#include "game/GameChunk.h"
#include "raycasting.h"
#include <optional>
#include <vector>

std::size_t GetGameDataSizeFn() { return sizeof(GameData); }

void RerenderDirtyChunks(World& world, EngineApi& api)
{
    for (auto& chunk : world.dirtyChunks)
    {
        api.RerenderChunk(chunk->blocks, chunk->x, chunk->z, chunk->handle);
    }
}

// If an input was pushed which would move the player, return true to signal the main loop to update the camera.
void UpdatePlayer(UserInput& input, Player& player, float deltaTime, EngineApi& api)
{
    static float movFactor {2.0f};
    float xMov {0.0f};
    float zMov{0.0f};
    float xCamMov{0.0f};
    float yCamMov{0.0f};
    float camChange = movFactor * deltaTime;
    if(input.keys & UserInput::W_PRESSED)
    {
        zMov -= camChange;
    }   
    if(input.keys & UserInput::S_PRESSED)
    {
        zMov += camChange;
    }   
    if(input.keys & UserInput::A_PRESSED)
    {
        xMov -= camChange;
    }   
    if(input.keys & UserInput::D_PRESSED)
    {
        xMov += camChange;
    }   
    if(input.mouse & UserInput::MOUSE_MOVED)
    {
        yCamMov = camChange * -input.mYRel;
        xCamMov = camChange * input.mXRel;
    }
    if(input.keys || input.mouse & UserInput::MOUSE_MOVED)
    {
        player.Update(xMov, zMov, xCamMov, yCamMov);
        api.SyncCameraToPlayer(player.viewMat);
    }
}

void UpdateGame(UserInput& input, Player& player, World& world, float deltaTime, EngineApi& api)
{
    UpdatePlayer(input, player, deltaTime, api);

    // If there is no chunk at the player position, spawn one in
    if(world.GetChunkSafe(player.pos) == nullptr)
    {
        world.CreateChunk(player.pos.x, player.pos.z, api);
    }
    
    // This half works, but the block destroyed isn't the one in front of the player
    // It's always slightly off to one side or another,
    // Or in some cases, massively off to one side.
    // Or in other cases, so far off to one side it seems like it doesn't work
    // WHY
    if (input.mouse & UserInput::LEFT_CLICK)
    {
        std::optional<Vector3> targetBlock{GetCoordinatesOfFirstBlockInRangeSafe(world, player, 10.0f)};
        if(targetBlock)
        {
            world.DestroyBlockUnsafe(targetBlock.value());
        }
    }

    // This works well
    // Good code.
    // Well done
    if(world.HasDirtyChunks())
    {
        RerenderDirtyChunks(world, api);
        world.ClearDirtyChunks();
    }
}

void RenderGame(World& world, EngineApi& api)
{
    for (auto& chunk : world.chunks)
    {
        api.DrawChunk(chunk.second.handle);
    }
}


void GameInitFn(GameMemory& memory, EngineApi& api)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    new (data) GameData{};
    data->player.pos = {8.0f, 256.f, 8.0f};
    data->player.target = {9.0f, 256.f, 9.0f};
    data->player.Init(api);
}

void GameUpdateFn(GameMemory& memory, EngineApi& api, UserInput& input, float deltaTime, bool skipRender)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    UpdateGame(input, data->player, data->world, deltaTime, api);
    
    if(!skipRender)
        RenderGame(data->world, api);
    memory.imGuiContext = Imgui_Frontend::Draw(data->player);
}