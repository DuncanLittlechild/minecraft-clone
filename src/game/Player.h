#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H

#include <cstdint>
struct Player {
    enum Flags : uint8_t {
        LEFTCLICKED = 0x01,
    };
    float x{.0f}, y{.0f}, z{.0f}; // Pos
    float xT{.0f}, yT{.0f}, zT{.0f}; // target
    uint8_t flags{0};

    void ResetFlags() { flags = 0; }
};


#endif // DL_SDL3GPU_PROJECT_TEMPLATE_PLAYER_H
