#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
#include <deque>
#include <unordered_map>
#include <utility>
#include <vector>
#include "NoiseManager.h"
#include "game_api.h"
#include "game/GameChunk.h"
#include "core/Matrix4x4.h"

// Finds the key for the chunk which the x and z coordinates are contained within
void GetChunkKeys(int& x, int& z);

// Functor used to hash the coordinate pair in unordered map
struct PairHash{
    std::size_t operator()(const std::pair<int, int>& pair) const
    {
        // Hash the first element
        size_t hash1 = std::hash<int>{}(pair.first);
        // Hash the second element
        size_t hash2 = std::hash<int>{}(pair.second);
        // Combine the two hash values
        return hash1
               ^ (hash2 + 0x9e3779b9 + (hash1 << 6)
                  + (hash1 >> 2));
    }
};

struct World {
    // Int values in key are x and z coordinates respectively
    std::unordered_map<std::pair<int, int>, GameChunk, PairHash> chunks{};
    std::vector<GameChunk*> dirtyChunks{};
    std::deque<std::pair<int, int>> chunkSpawnQueue{};
    int pChunkX {};
    int pChunkZ {};
    NoiseManager noiseManager{};
    int initialSeed {};

    void Init(unsigned long seed);

    GameChunk* GetChunkSafe(const int x, const int z);
    GameChunk& GetChunkUnsafe(const int x, const int z);

    BlockType::Type* GetBlockSafe(const Vector3&);
    BlockType::Type& GetBlockUnsafe(const Vector3&);

    void MarkChunkDirty(GameChunk&);

    void DestroyBlockSafe(const Vector3&);
    void DestroyBlockUnsafe(const Vector3&);

    void PlaceBlockUnsafe(const Vector3&, Block);

    void MarkExistingNeighboursDirtyChunk(int, int);
    void MarkExistingNeighboursDirtyBlock(int, int);
    void RecreateChunk(int, int, int, EngineApi&);
    void CreateChunk(int, int, EngineApi&);

    void ProcessSpawnQueue();

    void Update(EngineApi&);

    bool HasDirtyChunks() { return dirtyChunks.size() != 0; }
    void ClearDirtyChunks() { dirtyChunks.resize(0); }
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
