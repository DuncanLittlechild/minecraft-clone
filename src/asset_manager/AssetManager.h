#ifndef DL_TEMPLATE_MESHES_H
#define DL_TEMPLATE_MESHES_H
#include "SDL3/SDL.h"
#include <array>
#include <unordered_map>
#include <vector>
#include "SDL3/SDL_gpu.h"
#include "game_api.h"
#include "asset_manager/meshes.h"


struct Material{
    SDL_FColor albedo {};
    SDL_FColor specular{};
    float shininess{};
};

struct AssetManager {
    SDL_GPUTexture* textureArray{nullptr};
    SDL_GPUTexture* depthTexture {nullptr};
    SDL_GPUGraphicsPipeline* mainPipeline{nullptr};
    SDL_GPUSampler* sampler {nullptr};

    // Handles mapped to chunks
    // todo: LOGIC TO LOAD AND UNLOAD CHUNKS
    std::unordered_map<int, GPUMesh> chunks{};

    // Not currently needed
    SDL_GPUBuffer* instanceBuffer{nullptr};
    SDL_GPUTransferBuffer* instanceTransferBuffer{nullptr};

    void Load(SDL_GPUDevice*, SDL_Window*);
    void RecreateDepthTexture(SDL_GPUDevice*, SDL_Window*);
    int CreateGPUChunk(SDL_GPUDevice*,BlockType::Type[CHUNKWIDTH][CHUNKHEIGHT][CHUNKDEPTH], int, int);
    //void UploadToInstanceBuffer();
};


#endif