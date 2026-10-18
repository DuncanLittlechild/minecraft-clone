#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_RAYCASTING_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_RAYCASTING_H

#include "SDL3/SDL_stdinc.h"
#include "core/Matrix4x4.h"
#include "game/GameData.h"
#include <optional>

struct Ray {
    Vector3 origin{};
    Vector3 dir{};
    Vector3 pos{};

    Ray(Vector3 g_start, Vector3 g_end)
        : origin{g_start}
        , dir {g_end - g_start}
        , pos{origin}
    {
        Vector3_Normalize(dir);
    }

    void Move(const float);
};

std::optional<Vector3> GetCoordinatesOfFirstBlockInRange(World&,Player&,float);

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_RAYCASTING_H
