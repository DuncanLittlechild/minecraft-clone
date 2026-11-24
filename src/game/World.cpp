#include "World.h"
#include "GameChunk.h"
#include "core/Matrix4x4.h"
#include "core/global_consts.hpp"
#include "game_api.h"
#include "imgui_global.h"
#include <array>
#include <cmath>
#include <utility>
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

void GetChunkKeys(int& x, int& z)
{
    x &= CHUNKPOSMASK;
    z &= CHUNKPOSMASK;
}

void World::Init(unsigned long seed)
{
    initialSeed = seed;
    noiseManager.Init(seed);
}

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

BlockType::Type* World::GetBlockSafe(const Vector3& bPos)
{
    int gX {static_cast<int>(std::floor(bPos.x))};
    int y {static_cast<int>(std::floor(bPos.y))};
    int gZ {static_cast<int>(std::floor(bPos.z))};

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
        return &chunk->blocks[x][y][z];
    }
    else
        return nullptr;
}

BlockType::Type& World::GetBlockUnsafe(const Vector3& bPos)
{
    int gX {static_cast<int>(std::floor(bPos.x))};
    int y {static_cast<int>(std::floor(bPos.y))};
    int gZ {static_cast<int>(std::floor(bPos.z))};

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

void World::DestroyBlockSafe(const Vector3& bPos)
{
    int x {static_cast<int>(std::floor(bPos.x))};
    int z {static_cast<int>(std::floor(bPos.z))};
    GameChunk* hostChunk{GetChunkSafe(x, z)};
    if(hostChunk == nullptr)
        return;
    BlockType::Type* bType {GetBlockSafe(bPos)};
    if(bType == nullptr)
        return;
    *bType = BlockType::NOBLOCK;
    // TODO: mark neighbouring chunks as dirty if needed
    MarkChunkDirty(*hostChunk);
    MarkExistingNeighboursDirtyBlock(x, z);
}

void World::DestroyBlockUnsafe(const Vector3& bPos)
{
    int x {static_cast<int>(std::floor(bPos.x))};
    int z {static_cast<int>(std::floor(bPos.z))};
    GameChunk& hostChunk {GetChunkUnsafe(x, z)};
    BlockType::Type& bType {GetBlockUnsafe(bPos)};
    bType = BlockType::NOBLOCK;

    MarkChunkDirty(hostChunk);
    MarkExistingNeighboursDirtyBlock(x, z);
}

void World::PlaceBlockUnsafe(const Vector3& bPos, Block block)
{
    int x {static_cast<int>(std::floor(bPos.x))};
    int z {static_cast<int>(std::floor(bPos.z))};
    GameChunk& hostChunk {GetChunkUnsafe(x, z)};
    BlockType::Type& bType {GetBlockUnsafe(bPos)};
    bType = block;

    MarkChunkDirty(hostChunk);
    MarkExistingNeighboursDirtyBlock(x, z);
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

// Marks all chunks which neighbour a destroyed or placed block as dirty
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

void World::GetChunkNeighbours(int x, int z,
    GameChunk** n,
    GameChunk** s,
    GameChunk** e,
    GameChunk** w)
{
    // Get chunks to the north, south, east and west if they exist
    *n = GetChunkSafe(x + CHUNKOFFSET[DIRECTION_NORTH][0], z + CHUNKOFFSET[DIRECTION_NORTH][1]);
    *s = GetChunkSafe(x + CHUNKOFFSET[DIRECTION_SOUTH][0], z + CHUNKOFFSET[DIRECTION_SOUTH][1]);
    *e = GetChunkSafe(x + CHUNKOFFSET[DIRECTION_EAST][0], z + CHUNKOFFSET[DIRECTION_EAST][1]);
    *w  = GetChunkSafe(x + CHUNKOFFSET[DIRECTION_WEST][0], z + CHUNKOFFSET[DIRECTION_WEST][1]);
}

void World::TransferChunk(ChunkTransfer* cTransfer, int xPos, int zPos)
{
    cTransfer->xPos = xPos;
    cTransfer->zPos = zPos;
    cTransfer->active = true;

    GameChunk *main, *n, *s, *e, *w;
    main = GetChunkSafe(xPos, zPos);
    GetChunkNeighbours(xPos, zPos, &n, &s, &e, &w);

    for(int x {0}; x < CHUNKX; ++x)
    {
        for (int y {0}; y < CHUNKY; ++y)
        {
            for (int z{0}; z < CHUNKZ; ++z)
            {
                const Block& b {main->blocks[x][y][z]};
                BlockTransfer& bT {cTransfer->main[x][y][z]};
                bT.faces = 0;
                bT.type = b;
                if(bT.type == BlockType::NOBLOCK)
                    continue;

                if(z == 0)
                {
                    if(n && !n->blocks[x][y][Z_MAXIDX])
                        bT.faces |= DIRECTIONBITFLAG_NORTH;
                }
                else if (z == Z_MAXIDX)
                {
                    if(s && !s->blocks[x][y][0])
                        bT.faces |= DIRECTIONBITFLAG_SOUTH;
                }
                else if(main->blocks[x][y][z - 1])
                    bT.faces |= DIRECTIONBITFLAG_NORTH;
                else if(main->blocks[x][y][z + 1])
                    bT.faces |= DIRECTIONBITFLAG_SOUTH;

                if(x == 0)
                {
                    if(w && !w->blocks[X_MAXIDX][y][z])
                        bT.faces |= DIRECTIONBITFLAG_WEST;
                }
                else if (x == X_MAXIDX)
                {
                    if(e && !e->blocks[0][y][z])
                        bT.faces |= DIRECTIONBITFLAG_EAST;
                }
                else if(main->blocks[x - 1][y][z])
                    bT.faces |= DIRECTIONBITFLAG_WEST;
                else if(main->blocks[x + 1][y][z])
                    bT.faces |= DIRECTIONBITFLAG_EAST;

                if (y == Y_MAXIDX)
                {
                    bT.faces |= DIRECTIONBITFLAG_UP;
                }
                else if(main->blocks[x][y + 1][z])
                    bT.faces |= DIRECTIONBITFLAG_UP;
                else if(main->blocks[x][y - 1][z])
                    bT.faces |= DIRECTIONBITFLAG_DOWN;
            }
        }
    }
}

void World::Update(ChunkTransfer* gen, ChunkTransfer* regen)
{
    if(chunkSpawnQueue.size() > 0)
    {
        std::pair<int,int>* chunk {&chunkSpawnQueue[0]};
        gen->handle = GetChunkHandle();
        gen->active = true;
        TransferChunk(gen, chunk->first, chunk->second);
        chunkSpawnQueue.pop_front();
    }  
    if (HasDirtyChunks())
    {
        GameChunk* chunk {dirtyChunks[0]};
        regen->handle = chunk->handle;
        regen->active = true;
        TransferChunk(regen, chunk->x, chunk->z);
        dirtyChunks.pop_front();
    }  
}

