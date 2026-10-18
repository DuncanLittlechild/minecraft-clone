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
        api.RerenderChunk(chunk->blocks, chunk->x, chunk->y, chunk->handle);
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
        player.x += xMov;
        player.z += zMov;
        player.xT += xCamMov;
        player.yT += yCamMov;
        api.UpdateCamera(xMov, zMov, xCamMov, yCamMov);
    }
}

void UpdateGame(UserInput& input, Player& player, World& world, float deltaTime, EngineApi& api)
{
    UpdatePlayer(input, player, deltaTime, api);
    
    if (input.mouse & UserInput::LEFT_CLICK)
    {
        std::optional<Vector3> targetBlock{GetCoordinatesOfFirstBlockInRange(world, player, 1.0f)};
        if(targetBlock)
        {
            world.DestroyBlockUnsafe(targetBlock.value());
        }
    }

    if(world.HasDirtyChunks())
    {
        RerenderDirtyChunks(world, api);
        world.ClearDirtyChunks();
    }
}

void RenderGame(World& world, EngineApi& api)
{
    api.DrawChunk(world.chunks[0][0].handle);
}


void GameInitFn(GameMemory& memory, EngineApi& api)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    new (data) GameData{};
    data->world.CreateChunk(0, 0, api);
}

void GameUpdateFn(GameMemory& memory, EngineApi& api, UserInput& input, float deltaTime, bool skipRender)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    UpdateGame(input, data->player, data->world, deltaTime, api);
    
    if(!skipRender)
        RenderGame(data->world, api);
    memory.imGuiContext = Imgui_Frontend::Draw();
}