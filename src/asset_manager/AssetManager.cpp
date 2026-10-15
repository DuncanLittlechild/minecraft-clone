#include "AssetManager.h"
#include <array>
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "asset_manager/create_gpu_assets.h"
#include "asset_manager/stb_utils.h"
#include "renderer/Vertex.h"
bool GPUMesh::UploadToGPU(SDL_GPUDevice* device, const Mesh& mesh)
{
    indexCount = mesh.indices.size();
    Uint32 verticesSize = sizeof(Vertex) * mesh.vertices.size();
    Uint32 indicesSize = sizeof(uint16_t) * mesh.indices.size();

    SDL_GPUBufferCreateInfo vertexCreateInfo {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = verticesSize,
    };
    SDL_GPUBufferCreateInfo indexCreateInfo {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = verticesSize,
    };

    vertices = SDL_CreateGPUBuffer(device,&vertexCreateInfo);
    if(vertices == nullptr)
    {
        SDL_Log("Couldn't create vertex buffer");
        return false;   
    }
    indices = SDL_CreateGPUBuffer(device,&indexCreateInfo);
    if(indices == nullptr)
    {
        SDL_Log("Couldn't create index buffer");
        return false;   
    }

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize + indicesSize
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
    void* transferPtr{SDL_MapGPUTransferBuffer(device, transferBuffer, false)};
    SDL_memcpy(transferPtr, mesh.vertices.data(), verticesSize);
    void* indexTransferPtr {(char*)transferPtr + verticesSize}; 
    SDL_memcpy(indexTransferPtr, mesh.indices.data(), indicesSize);
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
        .buffer = vertices,
        .offset = 0,
        .size = verticesSize
    };
    SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, true);

    SDL_GPUTransferBufferLocation indexBufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = verticesSize
    };
    SDL_GPUBufferRegion indexBufferRegion {
        .buffer = indices,
        .offset = 0,
        .size = indicesSize
    };
    SDL_UploadToGPUBuffer(copyPass, &indexBufferLocation, &indexBufferRegion, true);

    SDL_EndGPUCopyPass(copyPass);
    if(!SDL_SubmitGPUCommandBuffer(uploadCommandBuff))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    return true;    
}

void AssetManager::Load(SDL_GPUDevice* device, SDL_Window* window)
{
    Mesh tmpMesh {
        .vertices {cubeColourVertices.begin(), cubeColourVertices.end()},
        .indices {cubeIndices.begin(), cubeIndices.end()}
    };
    cubeMesh.UploadToGPU(device, tmpMesh);

    GPU_Create::BlockPipeline(&mainPipeline, device, window);

    RecreateDepthTexture(device, window);

    GPU_Create::TextureArray(device, &textureArray);

    GPU_Create::Sampler(&sampler, device);

    //GPU_Create::InstanceBufferAndTransferBuffer(device, &instanceBuffer, &instanceTransferBuffer);
}

void AssetManager::RecreateDepthTexture(SDL_GPUDevice* device, SDL_Window* window)
{
    if(depthTexture != nullptr)
        SDL_ReleaseGPUTexture(device, depthTexture);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    GPU_Create::DepthTexture(device, &depthTexture, w, h);
}
/*
void AssetManager::UploadToInstanceBuffer(SDL_GPUDevice* device, Uint32 size, InstanceData* data, Uint32 dataCount)
{
    std::size_t dataSize{dataCount * sizeof(InstanceData)};
    // transfer info into the transfer buffer
    void* vertexTransferPtr1{SDL_MapGPUTransferBuffer(device, instanceTransferBuffer, false)};
    SDL_memcpy(vertexTransferPtr1, data, dataSize);
    SDL_UnmapGPUTransferBuffer(device, instanceTransferBuffer);

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
}*/
