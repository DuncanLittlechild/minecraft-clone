#ifndef DL_TEMPLATE_GAMEAPI_H
#define DL_TEMPLATE_GAMEAPI_H
#include "core/Matrix4x4.h"
#include "core/global_consts.hpp"
#include "imgui_internal.h"
#include <cstddef>
#include <cstdint>


enum Direction {
    DIRECTION_NORTH,  // -z
    DIRECTION_SOUTH,  // +z
    DIRECTION_EAST,   // +x
    DIRECTION_WEST,   // -x
    DIRECTION_CARDINAL_MAX,
    DIRECTION_UP = DIRECTION_CARDINAL_MAX,     // +y
    DIRECTION_DOWN,   // -y
    DIRECTION_MAX,
    DIRECTION_MIN = 0
};

enum DirectionBitflags {
    DIRECTIONBITFLAG_NORTH  = 1 << DIRECTION_NORTH, // -z
    DIRECTIONBITFLAG_SOUTH  = 1 << DIRECTION_SOUTH, // +z
    DIRECTIONBITFLAG_EAST   = 1 << DIRECTION_EAST, // +x
    DIRECTIONBITFLAG_WEST   = 1 << DIRECTION_WEST, // -x
    DIRECTIONBITFLAG_UP     = 1 << DIRECTION_UP, // +y
    DIRECTIONBITFLAG_DOWN   = 1 << DIRECTION_DOWN,  // -y
    DIRECTIONBITFLAG_COUNT
};


namespace BlockType{
    enum Type : uint8_t {
        DIRT,
        SAND,
        STONE,
        GRASS,
        LOG,
        TYPE_COUNT,
        MIN_TYPE = 0,
        NOBLOCK
    };
}

using Block = BlockType::Type;

struct BlockTransfer {
    Block type {BlockType::NOBLOCK};
    uint8_t faces{0};
};

struct ChunkTransfer {
    BlockTransfer main[CHUNKX][CHUNKY][CHUNKZ];
    int xPos{0};
    int zPos{0};
    int handle{0};
    bool active{false};
};

struct EngineApi {
    void (*DrawChunk)(int);
    // MUST be called before game start, as also initialises camera matrices
    void (*SyncCameraToPlayer)(const Matrix4x4&);
    void (*ToggleRelativeMouse)();
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
        R_PRESSED = 0x01'00
    };
    enum Mouse : MType {
        LEFT_CLICK = 0x01,
        RIGHT_CLICK = 0x02,
        MIDDLE_CLICK = 0x04,
        MOUSE_MOVED = 0x08
    };
    KType keys{0};
    MType mouse{0};
    float mXAbs{.0f};
    float mYAbs{.0f};
    float mXRel{.0f};
    float mYRel{.0f};
};

struct GameState;

struct GameMemory {
    ChunkTransfer* chunkGen{nullptr};
    ChunkTransfer* chunkRegen{nullptr};
    GameState* state{nullptr};
    int seed {5};
    std::size_t gameDataSize{0};
    void* gameData{nullptr};

    ImGuiContext* imGuiContext{nullptr};
};

// Both of these are located in game/game.cpp
void GameInitFn(GameMemory&, EngineApi&);
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