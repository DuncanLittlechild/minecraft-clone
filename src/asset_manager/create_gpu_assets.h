#ifndef DL_CHESS_CREATE_GPU_ASSETS_H
#define DL_CHESS_CREATE_GPU_ASSETS_H
#include <span>
#include <vector>
#include <array>
#include "SDL3/SDL.h"
#include <string>
#include "SDL3/SDL_gpu.h"
#include "asset_manager/Vertex.h"
#include "game_api.h"

struct DL_ShaderInfo{
    std::string shaderFilename {};
    Uint32 numSamplers{};
    Uint32 numStorageTextures{};
    Uint32 numStorageBuffers{};
    Uint32 numUniformBuffers{};
};

// Creates specifc assets needed for the renderer to function, and to be held on the asset manager
// None of these are generic functions despite their names.
namespace GPU_Create {
    bool Texture(SDL_GPUDevice*, SDL_Surface* src, SDL_GPUTexture**dst);

    bool TextureArray(SDL_GPUDevice*, SDL_GPUTexture**);

    bool VertexBuffer(SDL_GPUDevice* device, const std::vector<Vertex>& vertices, SDL_GPUBuffer** buffer);

    bool IndexBuffer(SDL_GPUDevice* device, const std::vector<uint32_t>& vertices, SDL_GPUBuffer** buffer, Uint32& indexCount);

    bool DepthTexture(SDL_GPUDevice* device, SDL_GPUTexture** depthTexture, int w, int h);

    bool Sampler(SDL_GPUSampler** sampler, SDL_GPUDevice* device);

    bool BlockPipeline(SDL_GPUGraphicsPipeline** pipeline, SDL_GPUDevice* device, SDL_Window* window);

    bool InstanceBufferAndTransferBuffer(SDL_GPUDevice*, SDL_GPUBuffer**, SDL_GPUTransferBuffer**);
}

#endif