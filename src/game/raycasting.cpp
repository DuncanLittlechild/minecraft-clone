#include "raycasting.h"
#include "GameData.h"
#include "game_api.h"
#include <optional>

void Ray::Move(const float dist)
{
    pos += dist * dir;
}

LOCAL constexpr float RAYINCREMENT{0.1f};
std::optional<Vector3> GetCoordinatesOfFirstBlockInRangeSafe(World& world, Player& player, float range)
{
    // Create a ray going out of the player in the direction faced
    Ray ray {player.pos,player.target};

    float distanceMoved {0.0f};
    bool insideBlock {false};
    // Increment said ray until it is inside a block or it has finished moving
    do{
        ray.Move(RAYINCREMENT);
        BlockType::Type* block { world.GetBlockSafe(ray.pos)};
        if (block != nullptr)
        {
            insideBlock = *block != BlockType::NOBLOCK;
        }
        distanceMoved += RAYINCREMENT;
    } while (distanceMoved < range && !insideBlock);

    if (insideBlock)
        return ray.pos;
    else
        return std::nullopt;
}