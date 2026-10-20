#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H

#include "core/Matrix4x4.h"
#include "game_api.h"
#include <cstdint>
struct Player {
    static constexpr Vector3 worldUp {0.0f, 1.0f, 0.0f};
    enum Flags : uint8_t {
        LEFTCLICKED = 0x01,
    };
	Vector3 pos{0.0f, .0f, 0.0f};
	Vector3 target{.0f, .0f, .0f};
    Matrix4x4 viewMat{};
    uint8_t flags{0};
    void UpdateViewMat();

    void Init(EngineApi&);
    void ResetFlags() { flags = 0; }
    void Update(float, float, float, float);
};


#endif // DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H
