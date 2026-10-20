#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_IMGUI_GLOBAL_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_IMGUI_GLOBAL_H

// to be deleted after testing
// This is just to provide a place ot put globals which vcan touch any part of the game

#include "core/Matrix4x4.h"
struct ImGuiGlobal{
    Vector3 lastRayStart{};
    Vector3 lastRayDir{};
    Vector3 lastRayEnd {};
    Vector3 lastRayBlock{};
} inline imGlob {};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_IMGUI_GLOBAL_H

