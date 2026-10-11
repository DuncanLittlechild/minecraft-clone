#include "dl_imgui_frontend.h"
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

ImGuiContext* Imgui_Frontend::Draw() {
    ImGui::NewFrame();
    ImGuiIO& io {ImGui::GetIO()};
    ImGui::Begin("Tab");
    ImGui::End();

    ImGui::Render();

    return ImGui::GetCurrentContext();
}