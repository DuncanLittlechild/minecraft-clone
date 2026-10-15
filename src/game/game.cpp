#include "GameData.h"
#include "game_api.h"
#include "imgui/dl_imgui_frontend.h"
#include "game/Block.h"

std::size_t GetGameDataSizeFn() { return sizeof(GameData); }

// If an input was pushed which would move the player, return true to signal the main loop to update the camera.
void ProcessInput(UserInput& input, Player& player, float deltaTime, EngineApi& api)
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
    if(input.keys & UserInput::UP_PRESSED)
    {
        yCamMov += camChange;
    }   
    if(input.keys & UserInput::DOWN_PRESSED)
    {
        yCamMov -= camChange;
    }   
    if(input.keys & UserInput::LEFT_PRESSED)
    {
        xCamMov -= camChange;
    }   
    if(input.keys & UserInput::RIGHT_PRESSED)
    {
        xCamMov += camChange;
    }   
    if(input.keys)
    {
        player.x += xMov;
        player.z += zMov;
        player.xT += xCamMov;
        player.yT += yCamMov;
        api.UpdateCamera(xMov, zMov, xCamMov, yCamMov);
    }
}

void GameUpdateFn(GameMemory& memory, EngineApi& api, UserInput& input, float deltaTime, bool skipRender)
{
    GameData* data {reinterpret_cast<GameData*>(memory.gameData)};
    ProcessInput(input, data->player, deltaTime, api);
    Block initBlock {BlockType::DIRT, 1.0f, 1.0f, 1.0f};
    api.DrawSprite(initBlock.ToRenderData());
    memory.imGuiContext = Imgui_Frontend::Draw();
}