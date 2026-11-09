#include "GameData.h"
#include "World.h"
#include "game_api.h"
#include "imgui/dl_imgui_frontend.h"
#include "game/GameChunk.h"
#include "raycasting.h"
#include <cmath>
#include <optional>
#include <vector>

std::size_t GetGameDataSizeFn() { return sizeof(GameData); }

bool menuMode {true};

void RerenderDirtyChunks(World& world, EngineApi& api)
{
    for (auto& chunk : world.dirtyChunks)
    {
        world.RecreateChunk(chunk->x, chunk->z, chunk->handle, api);
    }
}


void UpdateSettings(UserInput& input, EngineApi& api)
{
    if(input.keys & UserInput::R_PRESSED)
    {
        api.ToggleRelativeMouse();
        menuMode = !menuMode;
        input.keys &= ~UserInput::R_PRESSED;
    }
}
// If an input was pushed which would move the player, return true to signal the main loop to update the camera.
void UpdatePlayer(UserInput& input, Player& player, float deltaTime, EngineApi& api)
{
    float xMov {0.0f};
    float zMov{0.0f};
    float xCamMov{0.0f};
    float yCamMov{0.0f};
    float camChange = player.speed * deltaTime;
    float camSense = player.rSpeed * deltaTime;
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
    if(input.mouse & UserInput::MOUSE_MOVED && !menuMode)
    {
        yCamMov = camChange * -input.mYRel;
        xCamMov = camChange * input.mXRel;
    }
    if(input.keys || input.mouse & UserInput::MOUSE_MOVED)
    {
        player.Update(xMov, zMov, xCamMov, yCamMov);
        api.SyncCameraToPlayer(player.viewMat);

        // Check if the player has moved between chunks
        player.xPrevPlayerChunk = player.xPlayerChunk;
        player.zPrevPlayerChunk = player.zPlayerChunk;
        player.xPlayerChunk = std::floor(player.pos.x);
        player.zPlayerChunk = std::floor(player.pos.z);
        GetChunkKeys(player.xPlayerChunk, player.zPlayerChunk);
    }
}

void PopulateChunks(Player& player, World& world, EngineApi& api)
{
    static int chunkOffsetFactorX {9 * CHUNKX};
    static int chunkOffsetFactorZ {9 * CHUNKZ};
    // Get all coordinates in a 9x9 grid with the centre on the player
    int xStart {player.xPlayerChunk - chunkOffsetFactorX};
    int xEnd {player.xPlayerChunk + chunkOffsetFactorX};
    int zStart {player.zPlayerChunk - chunkOffsetFactorZ};
    int zEnd {player.zPlayerChunk + chunkOffsetFactorZ};

    for (int x {xStart}; x <= xEnd; x += CHUNKX)
    {
        for (int z {zStart}; z <= zEnd; z += CHUNKZ)
        {
            if(world.GetChunkSafe(x, z) == nullptr)
            {
                world.chunkSpawnQueue.emplace_back(x, z);
            }
        }
    }
}

void UpdateGame(UserInput& input, Player& player, World& world, float deltaTime, EngineApi& api)
{
    UpdatePlayer(input, player, deltaTime, api);
    world.Update(api);
    UpdateSettings(input, api);
    if(player.xPlayerChunk != player.xPrevPlayerChunk || player.zPlayerChunk != player.zPrevPlayerChunk)
    {
        PopulateChunks(player, world, api);
    }
    
    // This half works, but the block destroyed isn't the one in front of the player
    // It's always slightly off to one side or another,
    // Or in some cases, massively off to one side.
    // Or in other cases, so far off to one side it seems like it doesn't work
    // WHY
    if (input.mouse & UserInput::LEFT_CLICK)
    {
        Vector3 bPos;
        if(GetCoordinatesOfFirstBlockInRangeSafe(world, player, 2.f, bPos))
        {
            world.DestroyBlockUnsafe(bPos);
        }
        else
        {
            Block* support {world.GetBlockSafe({bPos.x, bPos.y - 1, bPos.z})};
            if(support != nullptr && *support != BlockType::NOBLOCK)
            {
                world.PlaceBlockUnsafe(bPos, BlockType::DIRT);
            }
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


void GameUpdateFn(GameMemory& memory, EngineApi& api, UserInput& input, float deltaTime, bool skipRender)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    UpdateGame(input, data->player, data->world, deltaTime, api);
    
    if(!skipRender)
        RenderGame(data->world, api);
    memory.imGuiContext = Imgui_Frontend::Draw(data->player);
}