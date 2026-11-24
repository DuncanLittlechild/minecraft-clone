#include "AssetManager.h"
#include <array>
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "asset_manager/create_gpu_assets.h"
#include "asset_manager/Vertex.h"
#include "core/global_consts.hpp"
#include "game_api.h"
#include "meshes.h"

GLOBAL Mesh G_MESH {};

constexpr std::array<DirectionBitflags, DIRECTIONBITFLAG_COUNT> FACEFLAGS {{
    DIRECTIONBITFLAG_NORTH,
    DIRECTIONBITFLAG_SOUTH,
    DIRECTIONBITFLAG_EAST,
    DIRECTIONBITFLAG_WEST,
    DIRECTIONBITFLAG_UP,
    DIRECTIONBITFLAG_DOWN
}};

void AssetManager::Load(SDL_GPUDevice* device, SDL_Window* window)
{
    GPU_Create::BlockPipeline(&mainPipeline, device, window);

    RecreateDepthTexture(device, window);

    GPU_Create::TextureArray(device, &textureArray);

    GPU_Create::Sampler(&sampler, device);

    G_MESH.vertices.reserve(4 * 6 * CHUNKMAXBLOCKS);
    G_MESH.indices.reserve(4 * 9 * CHUNKMAXBLOCKS);

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

LOCAL void GenerateGlobalChunkMesh(ChunkTransfer* cTransfer)
{
    G_MESH.vertices.clear();
    G_MESH.indices.clear();

    int meshCount {0};
    int cX {cTransfer->xPos};
    int cZ {cTransfer->zPos};
    for(int x {0}; x < CHUNKX; ++x)
    {
        for (int y {0}; y < CHUNKY; ++y)
        {
            for (int z {0}; z < CHUNKZ; ++z)
            {
                BlockTransfer& bT {cTransfer->main[x][y][z]};
                if(!bT.type)
                    continue;

                for(auto face : CubeFace::FACES)
                {
                    if(!(bT.faces & ~(1<<face)))
                        continue;
                    int textureIndex {};
                    if(MULTIFACEBLOCKS.contains(bT.type))
                        textureIndex = static_cast<int>(MULTIFACEBLOCKS.at(bT.type).at(face));
                    else
                        textureIndex = static_cast<int>(bT.type);
                    std::size_t indexInc {CUBEVERTICES.size() * meshCount++};

                    for(auto& vert : CUBEVERTICES[face])
                    {
                        Vertex tmpV {vert};
                        tmpV.x += x + cX;
                        tmpV.y += y;
                        tmpV.z += z + cZ;
                        tmpV.d = textureIndex;
                        G_MESH.vertices.push_back(tmpV);
                    }
                    for(auto idx : CUBEFACEINDICES)
                    {
                        G_MESH.indices.push_back(idx + indexInc);
                    }
                }
            }
        }
    }
}

void AssetManager::GenerateGPUChunk(
    SDL_GPUDevice* device,
    ChunkTransfer* cTransfer
    )
{
    GenerateGlobalChunkMesh(cTransfer);
    int handle {cTransfer->handle};
    chunks[handle].Clear(device);
    chunks[handle].UploadToGPU(device, G_MESH);
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
