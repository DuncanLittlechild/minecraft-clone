#include "dl_imgui_frontend.h"
#include "game_api.h"
#include "imgui.h"

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
    ImGui::NewFrame();
    ImGuiIO& io {ImGui::GetIO()};
    ImGui::Begin("Player stats");
    ImGui::Text("Position - x:%f, y:%f, z:%f", player.pos.x, player.pos.y, player.pos.z);
    ImGui::Text("Target - x:%f, y:%f, z:%f", player.target.x, player.target.y, player.target.z);
    ImGui::End();

    //ImGui::Render();

    return ImGui::GetCurrentContext();
}