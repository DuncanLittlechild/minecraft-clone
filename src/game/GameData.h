#ifndef DL_TEMPLATE_GAMEDATA_H
#define DL_TEMPLATE_GAMEDATA_H
#include "GameChunk.h"
#include "Player.h"
#include "core/Matrix4x4.h"
#include <unordered_map>
#include "game/World.h"

struct GameData{
    Player player{};
    World world{};
};
#endif