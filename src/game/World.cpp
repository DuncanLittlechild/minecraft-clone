#include "World.h"
#include "GameChunk.h"
#include "core/Matrix4x4.h"
#include "game_api.h"
#include "imgui_global.h"
// When anded with a world coordinate, this returns the index of the
// chunk which contains said coordinate
LOCAL constexpr int CHUNKPOSMASK {~0b1111};

GameChunk* World::GetChunkSafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.z & CHUNKPOSMASK};
    if(chunks.contains(cCoords[0]) && chunks.at(cCoords[0]).contains(cCoords[1]))
        return &chunks[cCoords[0]][cCoords[1]];
    else
        return nullptr;
}

GameChunk& World::GetChunkUnsafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.z & CHUNKPOSMASK};
    return chunks[cCoords[0]][cCoords[1]];
}

BlockType::Type* World::GetBlockSafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.z & CHUNKPOSMASK};
    if(!(chunks.contains(cCoords[0]) && chunks.at(cCoords[0]).contains(cCoords[1])))
        return nullptr;
    GameChunk* chunk {&chunks.at(cCoords[0]).at(cCoords[1])};
    int x {static_cast<int>(coord.x - cCoords[0])};
    int y {static_cast<int>(coord.y)};
    int z {static_cast<int>(coord.z - cCoords[1])};

    if(chunk != nullptr 
        && x >= 0 && x < CHUNKX && y >= 0 && y < CHUNKY && z >= 0 && z < CHUNKZ)
    {
        // TODO: remove when target bug fixed
        imGlob.lastRayBlock = {(float)x, (float)y, (float)z};
        return &chunk->blocks[x][y][z];
    }
    else
        return nullptr;
}

BlockType::Type& World::GetBlockUnsafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.z & CHUNKPOSMASK};
    GameChunk& chunk {chunks[cCoords[0]][cCoords[1]]};
    int x {static_cast<int>(coord.x - cCoords[0])};
    int y {static_cast<int>(coord.y)};
    int z {static_cast<int>(coord.z - cCoords[1])};
    return chunk.blocks[x][y][z];
}

void World::MarkChunkDirty(GameChunk& chunk)
{
    dirtyChunks.push_back(&chunk);
}

void World::DestroyBlockSafe(const Vector3& block)
{
    GameChunk* hostChunk{GetChunkSafe(block)};
    if(hostChunk == nullptr)
        return;
    BlockType::Type* bType {GetBlockSafe(block)};
    if(bType == nullptr)
        return;
    *bType = BlockType::NOBLOCK;
    MarkChunkDirty(*hostChunk);
}

void World::DestroyBlockUnsafe(const Vector3& block)
{
    GameChunk& hostChunk {GetChunkUnsafe(block)};
    BlockType::Type& bType {GetBlockUnsafe(block)};
    bType = BlockType::NOBLOCK;
    MarkChunkDirty(hostChunk);
}

void World::CreateChunk(int x, int y, EngineApi& api)
{
    chunks[x][y] = GameChunk{};
    chunks[x][y].Init(x, y, api);
}
