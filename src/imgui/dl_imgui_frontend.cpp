#include "dl_imgui_frontend.h"
#include "core/Matrix4x4.h"
#include "game_api.h"
#include "imgui.h"
#include "game/imgui_global.h"

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

ImGuiContext* Imgui_Frontend::Draw(Player& player) {
    Vector3 dir {Vector3_Normalize(player.target - player.pos)};
    ImGui::NewFrame();
    ImGuiIO& io {ImGui::GetIO()};
    ImGui::Begin("Player stats");
    ImGui::Text("Position - x:%f, y:%f, z:%f", player.pos.x, player.pos.y, player.pos.z);
    ImGui::Text("Dir - x:%f, y:%f, z:%f", dir.x, dir.y, dir.z);
    ImGui::Text("Target - x:%f, y:%f, z:%f", player.target.x, player.target.y, player.target.z);
    ImGui::End();
    ImGui::Begin("Last Ray");
    ImGui::Text("Origin - x:%f, y:%f, z:%f", imGlob.lastRayStart.x, imGlob.lastRayStart.y, imGlob.lastRayStart.z);
    ImGui::Text("Direction - x:%f, y:%f, z:%f", imGlob.lastRayDir.x, imGlob.lastRayDir.y, imGlob.lastRayDir.z);
    ImGui::Text("End - x:%f, y:%f, z:%f", imGlob.lastRayEnd.x, imGlob.lastRayEnd.y, imGlob.lastRayEnd.z);
    ImGui::Text("Block - x:%f, y:%f, z:%f", imGlob.lastRayBlock.x, imGlob.lastRayBlock.y, imGlob.lastRayBlock.z);
    ImGui::End();

    //ImGui::Render();

    return ImGui::GetCurrentContext();
}