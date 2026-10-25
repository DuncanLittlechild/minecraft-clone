#include "World.h"
#include "GameChunk.h"
#include "core/Matrix4x4.h"
#include "core/global_consts.h"
#include "game_api.h"
#include "imgui_global.h"
#include <array>
// When anded with a world coordinate, this returns the index of the
// chunk which contains said coordinate
// this is a bit of a load-bearing calculation - if this is slightly off,
// the chunk coordinate system will fail and thus everything will fail
LOCAL constexpr int CHUNKPOSMASK {~0b1111};

// Stores x and z offsets needed to get the chunk in that direction
LOCAL constexpr std::array<int[2], DIRECTION_CARDINAL_MAX> CHUNKOFFSET{{
    {0, -CHUNKZ},
    {0, CHUNKZ},
    {CHUNKX, 0},
    {-CHUNKX, 0}

}};

// Stores x and z offsets needed to get the block in that direction
LOCAL constexpr std::array<int[2], DIRECTION_CARDINAL_MAX> BLOCKOFFSET{{
    {0, -1},
    {0, 1},
    {1, 0},
    {-1, 0}

}};



GameChunk* World::GetChunkSafe(int x, int z)
{
    int cCoords[2] {x & CHUNKPOSMASK, z & CHUNKPOSMASK};
    if(chunks.contains({cCoords[0],cCoords[1]}))
        return &chunks[{cCoords[0], cCoords[1]}];
    else
        return nullptr;
}

GameChunk& World::GetChunkUnsafe(int x, int z)
{
    int cCoords[2] {x & CHUNKPOSMASK, z & CHUNKPOSMASK};
    return chunks[{cCoords[0], cCoords[1]}];
}

BlockType::Type* World::GetBlockSafe(const Vector3& coord)
{
    int cCoords[2] {(int)coord.x & CHUNKPOSMASK, (int)coord.z & CHUNKPOSMASK};
    if(!(chunks.contains({cCoords[0],cCoords[1]})))
        return nullptr;
    GameChunk* chunk {&chunks.at({cCoords[0],cCoords[1]})};
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
    GameChunk& chunk {chunks[{cCoords[0],cCoords[1]}]};
    int x {static_cast<int>(coord.x - cCoords[0])};
    int y {static_cast<int>(coord.y)};
    int z {static_cast<int>(coord.z - cCoords[1])};
    return chunk.blocks[x][y][z];
}

void World::MarkChunkDirty(GameChunk& chunk)
{
    dirtyChunks.push_back(&chunk);
}


void World::MarkExistingNeighboursDirtyChunk(int x, int z)
{
    for (auto& offset : CHUNKOFFSET)
    {
        GameChunk* chunk {GetChunkSafe(offset[0] + x, offset[1] + z)};
        if(chunk != nullptr)
        {
            MarkChunkDirty(*chunk);
        }
    }
}

void World::MarkExistingNeighboursDirtyBlock(int x, int z)
{
    if(x == 0)
    {
        GameChunk* w {GetChunkSafe(x, z)};
        if(w != nullptr)
        {
            MarkChunkDirty(*w);
        }
    }
    else if(x == (CHUNKX - 1))
    {
        GameChunk* e {GetChunkSafe(x, z)};
        if(e != nullptr)
        {
            MarkChunkDirty(*e);
        }
    }
    if(z == 0)
    {
        GameChunk* n {GetChunkSafe(x, z)};
        if(n != nullptr)
        {
            MarkChunkDirty(*n);
        }
    }
    else if(z == CHUNKZ - 1)
    {
        GameChunk* s {GetChunkSafe(x, z)};
        if(s != nullptr)
        {
            MarkChunkDirty(*s);
        }
    }
}

void World::DestroyBlockSafe(const Vector3& block)
{
    GameChunk* hostChunk{GetChunkSafe(static_cast<int>(block.x), static_cast<int>(block.z))};
    if(hostChunk == nullptr)
        return;
    BlockType::Type* bType {GetBlockSafe(block)};
    if(bType == nullptr)
        return;
    *bType = BlockType::NOBLOCK;
    // TODO: mark neighbouring chunks as dirty if needed
    MarkChunkDirty(*hostChunk);
    //MarkExistingNeighboursDirtyBlock(block.x, block.z);
}

void World::DestroyBlockUnsafe(const Vector3& block)
{
    GameChunk& hostChunk {GetChunkUnsafe(static_cast<int>(block.x), static_cast<int>(block.z))};
    BlockType::Type& bType {GetBlockUnsafe(block)};
    bType = BlockType::NOBLOCK;
    int x {static_cast<int>(block.x)};
    int z {static_cast<int>(block.z)};

    MarkChunkDirty(hostChunk);
    //MarkExistingNeighboursDirtyBlock(block.x, block.z);
}

LOCAL void GetChunkNeighbours(int x, int z,
    GameChunk** n,
    GameChunk** s,
    GameChunk** e,
    GameChunk** w,
    World& world)
{
    // Get chunks to the north, south, east and west if they exist
    *n = world.GetChunkSafe(x + CHUNKOFFSET[DIRECTION_NORTH][0], z + CHUNKOFFSET[DIRECTION_NORTH][1]);
    *s = world.GetChunkSafe(x + CHUNKOFFSET[DIRECTION_SOUTH][0], z + CHUNKOFFSET[DIRECTION_SOUTH][1]);
    *e = world.GetChunkSafe(x + CHUNKOFFSET[DIRECTION_EAST][0], z + CHUNKOFFSET[DIRECTION_EAST][1]);
    *w  = world.GetChunkSafe(x + CHUNKOFFSET[DIRECTION_WEST][0], z + CHUNKOFFSET[DIRECTION_WEST][1]);
}

void World::RecreateChunk(int x, int z, int handle, EngineApi& api)
{
    // Rounds to the nearest multiple of 16
    x &= CHUNKPOSMASK;
    z &= CHUNKPOSMASK;
    GameChunk *n, *s, *e, *w;
    GetChunkNeighbours(x, z, &n, &s, &e, &w, *this);

    // Check to see if the returned values are null before creating the chunk
    api.RerenderChunk(
        chunks[{x,z}].blocks,
        n != nullptr ? n->blocks : nullptr,
        s != nullptr ? s->blocks : nullptr,
        e != nullptr ? e->blocks : nullptr,
        w != nullptr ? w->blocks : nullptr,
        x,z,
        handle
    );
}

void World::CreateChunk(int x, int z, EngineApi& api)
{
    // Rounds to the nearest multiple of 16
    x &= CHUNKPOSMASK;
    z &= CHUNKPOSMASK;
    chunks[{x,z}] = GameChunk{};
    chunks[{x,z}].Init(x, z, api);
    // Get neighbours to the current chunk
    GameChunk *n, *s, *e, *w;
    GetChunkNeighbours(x, z, &n, &s, &e, &w, *this);
    // Check to see if the returned values are null before creating the chunk
    chunks[{x,z}].handle = api.GetChunkHandle(
        chunks[{x,z}].blocks,
        n != nullptr ? n->blocks : nullptr,
        s != nullptr ? s->blocks : nullptr,
        e != nullptr ? e->blocks : nullptr,
        w != nullptr ? w->blocks : nullptr,
        x,z
    );

    // Mark neighbouring chunks as dirty, if they exist
    MarkExistingNeighboursDirtyChunk(x, z);
}
