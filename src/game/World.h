#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
#include <unordered_map>
#include <vector>
#include "game/GameChunk.h"
#include "core/Matrix4x4.h"


struct World {
    std::unordered_map<int, std::unordered_map<int, GameChunk>> chunks{};
    std::vector<GameChunk*> dirtyChunks{};

    GameChunk* GetChunkSafe(const Vector3&);
    GameChunk& GetChunkUnsafe(const Vector3&);

    BlockType::Type* GetBlockSafe(const Vector3&);
    BlockType::Type& GetBlockUnsafe(const Vector3&);

    void MarkChunkDirty(GameChunk&);

    void DestroyBlockUnsafe(const Vector3&);

    void CreateChunk(int, int, EngineApi&);

    bool HasDirtyChunks() {return dirtyChunks.size(); }
    void ClearDirtyChunks() { dirtyChunks.resize(0); }
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_WORLD_H
