#ifndef DL_CHESS_CREATE_GPU_ASSETS_H
#define DL_CHESS_CREATE_GPU_ASSETS_H
#include <span>
#include <vector>
#include "SDL3/SDL.h"
#include <string>
#include "core/Vertex.h"
#include "core/Matrix4x4.h"

struct DL_ShaderInfo{
    std::string shaderFilename {};
    Uint32 numSamplers{};
    Uint32 numStorageTextures{};
    Uint32 numStorageBuffers{};
    Uint32 numUniformBuffers{};
};

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const DL_ShaderInfo& dlShaderInfo);

template <typename T>
bool CreateGPUBuffer(SDL_GPUDevice* device, std::span<const T> vertices, SDL_GPUBufferUsageFlags usage, SDL_GPUBuffer** buffer, Uint32* indexCount = nullptr)
{
    Uint32 verticesSize = sizeof(T) * vertices.size();

    SDL_GPUBufferCreateInfo vertexCreateInfo {
        .usage = usage,
        .size = verticesSize,
    };

    *buffer = SDL_CreateGPUBuffer(device,&vertexCreateInfo);
    if(*buffer == nullptr)
    {
        SDL_Log("Couldn't create buffer");
        return false;   
    }

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize
    };

    SDL_GPUTransferBuffer* transferBuffer{
        SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo)
    };

    if(transferBuffer == nullptr)
    {
        SDL_Log("Couldn't create transfer buffer");
        return false;
    }

    // transfer info into the transfer buffer
    void* vertexTransferPtr1{SDL_MapGPUTransferBuffer(device, transferBuffer, false)};
    SDL_memcpy(vertexTransferPtr1, vertices.data(), verticesSize);
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCommandBuff {
        SDL_AcquireGPUCommandBuffer(device)
    };
    if (uploadCommandBuff == nullptr)
    {
        SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass* copyPass {SDL_BeginGPUCopyPass(uploadCommandBuff)};

    SDL_GPUTransferBufferLocation bufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion bufferRegion {
        .buffer = *buffer,
        .offset = 0,
        .size = verticesSize
    };

    SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, true);

    SDL_EndGPUCopyPass(copyPass);
    if(!SDL_SubmitGPUCommandBuffer(uploadCommandBuff))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    if(usage == SDL_GPU_BUFFERUSAGE_INDEX)
    {
        if(indexCount != nullptr)
        {
            *indexCount = vertices.size();
        }
        else 
        {
            SDL_Log("Index count must not be null when creating index buffer");
            return false;
        }
    }

    return true;        
}

bool CreateGPUVertexBuffer(SDL_GPUDevice* device, const std::vector<Vertex>& vertices, SDL_GPUBuffer** buffer);

bool CreateGPUIndexBuffer(SDL_GPUDevice* device, const std::vector<uint16_t>& vertices, SDL_GPUBuffer** buffer, Uint32& indexCount);

bool CreateDepthTexture(SDL_GPUDevice* device, SDL_GPUTexture** depthTexture, int w, int h);

bool CreateGPUSampler(SDL_GPUSampler** sampler, SDL_GPUDevice* device);

bool CreatePipeline(SDL_GPUGraphicsPipeline** pipeline, SDL_GPUDevice* device, SDL_Window* window, DL_ShaderInfo& vertexShaderInfo, DL_ShaderInfo& fragmentShaderInfo);

#endif