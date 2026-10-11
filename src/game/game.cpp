#include "GameData.h"
#include "game_api.h"
#include "imgui/dl_imgui_frontend.h"
#include "game/Block.h"

std::size_t GetGameDataSizeFn() { return sizeof(GameData); }

void GameUpdateFn(GameMemory& memory, EngineApi& api, UserInput& input, float deltaTime, bool skipRender)
{
    Block initBlock {Block::DIRT, .0f, .0f, .0f};
    api.DrawSprite(initBlock.ToRenderData());
    memory.imGuiContext = Imgui_Frontend::Draw();
}