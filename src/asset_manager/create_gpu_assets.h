#ifndef DL_CHESS_CREATE_GPU_ASSETS_H
#define DL_CHESS_CREATE_GPU_ASSETS_H
#include <span>
#include <vector>
#include "SDL3/SDL.h"
#include <string>
#include "renderer/Vertex.h"

struct DL_ShaderInfo{
    std::string shaderFilename {};
    Uint32 numSamplers{};
    Uint32 numStorageTextures{};
    Uint32 numStorageBuffers{};
    Uint32 numUniformBuffers{};
};

namespace GPU_Create {
    bool Texture(SDL_GPUDevice*, SDL_Surface* src, SDL_GPUTexture**dst);

    bool VertexBuffer(SDL_GPUDevice* device, const std::vector<Vertex>& vertices, SDL_GPUBuffer** buffer);

    bool IndexBuffer(SDL_GPUDevice* device, const std::vector<uint16_t>& vertices, SDL_GPUBuffer** buffer, Uint32& indexCount);

    bool DepthTexture(SDL_GPUDevice* device, SDL_GPUTexture** depthTexture, int w, int h);

    bool Sampler(SDL_GPUSampler** sampler, SDL_GPUDevice* device);

    bool Pipeline(SDL_GPUGraphicsPipeline** pipeline, SDL_GPUDevice* device, SDL_Window* window, DL_ShaderInfo& vertexShaderInfo, DL_ShaderInfo& fragmentShaderInfo);
}

#endif