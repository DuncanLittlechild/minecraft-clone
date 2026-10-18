#include "World.h"
#include "GameChunk.h"
// When anded with a world coordinate, this returns the index of the
// chunk which contains said coordinate
LOCAL constexpr int CHUNKPOSMASK {~0b1111};

GameChunk* World::GetChunkSafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.y & CHUNKPOSMASK};
    if(chunks.contains(cCoords[0]) && chunks.at(cCoords[0]).contains(cCoords[1]))
        return &chunks[cCoords[0]][cCoords[1]];
    else
        return nullptr;
}

GameChunk& World::GetChunkUnsafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.y & CHUNKPOSMASK};
    return chunks[cCoords[0]][cCoords[1]];
}

BlockType::Type* World::GetBlockSafe(const Vector3& coord)
{
    GameChunk* chunk {GetChunkSafe(coord)};
    int x {static_cast<int>(coord.x)};
    int y {static_cast<int>(coord.y)};
    int z {static_cast<int>(coord.z)};

    if(chunk != nullptr 
        && x >= 0 && x < CHUNKX && y >= 0 && y < CHUNKY && z >= 0 && z < CHUNKZ)
    {
        return &chunk->blocks[x][y][z];
    }
    else
        return nullptr;
}

BlockType::Type& World::GetBlockUnsafe(const Vector3& coord)
{
    GameChunk& chunk {GetChunkUnsafe(coord)};
    int x {static_cast<int>(coord.x)};
    int y {static_cast<int>(coord.y)};
    int z {static_cast<int>(coord.z)};
    return chunk.blocks[x][y][z];
}

void World::MarkChunkDirty(GameChunk& chunk)
{
    dirtyChunks.push_back(&chunk);
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
