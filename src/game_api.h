#ifndef DL_TEMPLATE_GAMEAPI_H
#define DL_TEMPLATE_GAMEAPI_H
#include "core/global_consts.h"
#include "imgui_internal.h"
#include <cstddef>
#include <cstdint>

namespace BlockType{
    enum Type : uint32_t {
        DIRT,
        SAND,
        STONE,
        TYPE_COUNT,
        MIN_TYPE = 0,
        NOBLOCK
    };
}

struct SpriteRenderData {
    enum Type {
        BLOCK,
        CHUNK
    } type{};
    float x{.0f}, y{.0f}, z{.0f}; // Position
    float scale{.0f}; // size relative to the basic mesh size
    uint32_t atlasHandle{0};
    uint8_t colourHandle{0};
    uint8_t layer{0};
};

struct UserInput {
    using KType = uint16_t;
    using MType = uint8_t;
    enum Key : KType {
        W_PRESSED= 0x00'01,
        A_PRESSED = 0x00'02,
        S_PRESSED = 0x00'04,
        D_PRESSED= 0x00'08,
        UP_PRESSED = 0x00'10,
        LEFT_PRESSED = 0x00'20,
        RIGHT_PRESSED = 0x00'40,
        DOWN_PRESSED = 0x00'80,
    };
    enum Mouse : MType {
        LEFT_CLICK = 0x01,
        RIGHT_CLICK = 0x02,
        MIDDLE_CLICK = 0x04,
    };
    KType keys{0};
    MType but{0};
    float mXAbs{.0f};
    float mYAbs{.0f};
    float mXRel{.0f};
    float mYRel{.0f};
};

struct EngineApi {
    void (*UpdateCamera)(float, float, float, float);
    int (*GetChunkHandle)(BlockType::Type[CHUNKX][CHUNKY][CHUNKZ], int, int);
    void (*DrawChunk)(int);
};

struct GameMemory {
    std::size_t gameDataSize{0};
    void* gameData{nullptr};

    ImGuiContext* imGuiContext{nullptr};
};

// Both of these are located in game/game.cpp
void GameInitFn(EngineApi& api);
void GameUpdateFn(GameMemory&, EngineApi&, UserInput&, float deltaTime, bool skipRender); 
std::size_t GetGameDataSizeFn();

/* Setup for hot reloading, only needed if that becomes more convenient than just re-running cmake
/// Declaring pointers to these two functions
// As these will be loaded into the host at runtime, they need to have names which are not mangled by 
// the compiler, hence extern "C", and the Fn suffix
extern "C" {
    typedef void GameUpdateFn(GameMemory*, EngineApi*, float deltaTime);
    typedef std::size_t GetGameDataSizeFn();
}
    */

#endif