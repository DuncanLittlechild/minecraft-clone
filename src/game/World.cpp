#include "World.h"
#include "GameChunk.h"
#include "core/Matrix4x4.h"
#include "core/global_consts.h"
#include "game_api.h"
#include "imgui_global.h"
#include <array>
#include <cmath>
// When anded with a world coordinate, this returns the index of the
// chunk which contains said coordinate
// this is a bit of a load-bearing calculation - if this is slightly off,
// the chunk coordinate system will fail and thus everything will fail
LOCAL constexpr int CHUNKPOSMASK {~0b1111};
LOCAL constexpr int BLOCKPOSMASK {0b1111};

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



GameChunk* World::GetChunkSafe(int gX, int gZ)
{
    int cX {gX & CHUNKPOSMASK};
    int cZ {gZ & CHUNKPOSMASK};
    if(chunks.contains({cX,cZ}))
        return &chunks[{cX, cZ}];
    else
        return nullptr;
}

GameChunk& World::GetChunkUnsafe(int gX, int gZ)
{
    int cX {gX & CHUNKPOSMASK};
    int cZ {gZ & CHUNKPOSMASK};
    int cCoords[2] {cX & CHUNKPOSMASK, cZ & CHUNKPOSMASK};
    return chunks[{cX, cZ}];
}

BlockType::Type* World::GetBlockSafe(const Vector3& coord)
{
    int gX {static_cast<int>(std::floor(coord.x))};
    int y {static_cast<int>(std::floor(coord.y))};
    int gZ {static_cast<int>(std::floor(coord.z))};

    int cX {gX & CHUNKPOSMASK};
    int cZ {gZ & CHUNKPOSMASK};

    if(!(chunks.contains({cX,cZ})))
        return nullptr;
    GameChunk* chunk {&chunks.at({cX, cZ})};
    int x {gX - cX};
    int z {gZ - cZ};

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
    int gX {static_cast<int>(std::floor(coord.x))};
    int y {static_cast<int>(std::floor(coord.y))};
    int gZ {static_cast<int>(std::floor(coord.z))};

    int cX {gX & CHUNKPOSMASK};
    int cZ {gZ & CHUNKPOSMASK};

    GameChunk& chunk {chunks[{cX,cZ}]};
    int x {gX - cX};
    int z {gZ - cZ};
    return chunk.blocks[x][y][z];
}

void World::MarkChunkDirty(GameChunk& chunk)
{
    dirtyChunks.push_back(&chunk);
}


void World::MarkExistingNeighboursDirtyChunk(int cX, int cZ)
{
    for (auto& offset : CHUNKOFFSET)
    {
        GameChunk* chunk {GetChunkSafe(offset[0] + cX, offset[1] + cZ)};
        if(chunk != nullptr)
        {
            MarkChunkDirty(*chunk);
        }
    }
}

void World::MarkExistingNeighboursDirtyBlock(int gX, int gZ)
{
    int x {gX & BLOCKPOSMASK};
    int z {gZ & BLOCKPOSMASK};
    if(x == 0)
    {
        GameChunk* w {GetChunkSafe(
            gX + BLOCKOFFSET[DIRECTION_WEST][0],
            gZ + BLOCKOFFSET[DIRECTION_WEST][1]
            )};
        if(w != nullptr)
        {
            MarkChunkDirty(*w);
        }
    }
    else if(x == (CHUNKX - 1))
    {
        GameChunk* e {GetChunkSafe(            
            gX + BLOCKOFFSET[DIRECTION_EAST][0],
            gZ + BLOCKOFFSET[DIRECTION_EAST][1]
        )};
        if(e != nullptr)
        {
            MarkChunkDirty(*e);
        }
    }
    if(z == 0)
    {
        GameChunk* n {GetChunkSafe(            
            gX + BLOCKOFFSET[DIRECTION_NORTH][0],
            gZ + BLOCKOFFSET[DIRECTION_NORTH][1]
        )};
        if(n != nullptr)
        {
            MarkChunkDirty(*n);
        }
    }
    else if(z == CHUNKZ - 1)
    {
        GameChunk* s {GetChunkSafe(            
            gX + BLOCKOFFSET[DIRECTION_SOUTH][0],
            gZ + BLOCKOFFSET[DIRECTION_SOUTH][1]
        )};
        if(s != nullptr)
        {
            MarkChunkDirty(*s);
        }
    }
}

void World::DestroyBlockSafe(const Vector3& block)
{
    int x {static_cast<int>(std::floor(block.x))};
    int z {static_cast<int>(std::floor(block.z))};
    GameChunk* hostChunk{GetChunkSafe(x, z)};
    if(hostChunk == nullptr)
        return;
    BlockType::Type* bType {GetBlockSafe(block)};
    if(bType == nullptr)
        return;
    *bType = BlockType::NOBLOCK;
    // TODO: mark neighbouring chunks as dirty if needed
    MarkChunkDirty(*hostChunk);
    MarkExistingNeighboursDirtyBlock(x, z);
}

void World::DestroyBlockUnsafe(const Vector3& block)
{
    int x {static_cast<int>(std::floor(block.x))};
    int z {static_cast<int>(std::floor(block.z))};
    GameChunk& hostChunk {GetChunkUnsafe(x, z)};
    BlockType::Type& bType {GetBlockUnsafe(block)};
    bType = BlockType::NOBLOCK;

    MarkChunkDirty(hostChunk);
    MarkExistingNeighboursDirtyBlock(x, z);
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
