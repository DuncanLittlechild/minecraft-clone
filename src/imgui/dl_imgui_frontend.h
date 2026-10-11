#ifndef DL_SDL3_IMGUI_UTILS_H
#define DL_SDL3_IMGUI_UTILS_H
// Only ever included in the game logic layer
// Is unaware of the specific backend being used, 
// which must be setup in the platform host layer
#include "imgui_internal.h"
namespace Imgui_Frontend{
    [[nodiscard("Creates context to be passed back to host")]] ImGuiContext* Draw();
}

#endif
