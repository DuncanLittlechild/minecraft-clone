#include "AssetManager.h"
#include <array>
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "asset_manager/create_gpu_assets.h"

void AssetManager::Load(SDL_GPUDevice* device, SDL_Window* window)
{
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

int AssetManager::CreateGPUChunk(SDL_GPUDevice* device,BlockType::Type blocks[CHUNKWIDTH][CHUNKHEIGHT][CHUNKDEPTH], int cX, int cY)
{
    // Gets a unique handle
    // NOTE: too many chunks could theoretically fuck this up
    auto GetNewChunkHandle {[](){
        static int handleCount{0};
        return handleCount++;
    }};
    // Modifies an inputted cube mesh to be in the position determined by both
    // the chunk's position and the cubes position within the chunk
    auto ModifyCubeMesh {[cX, cY](Mesh& mesh, int meshCount, BlockType::Type bType, int x, int y, int z){
        std::size_t indexInc {mesh.vertices.size() * meshCount};
        for(auto& vert : mesh.vertices)
        {
            vert.x += x + cX;
            vert.y += y + cY;
            vert.z += z;
            vert.d = static_cast<int>(bType);
        }

        for(auto& ind : mesh.indices)
        {
            ind += indexInc;
        }
    }};

    Mesh cMesh{};
    int meshCount {0};
    for(int x {0}; x < CHUNKHEIGHT; ++x)
    {
        for (int y{0}; y < CHUNKWIDTH; ++y)
        {
            for (int z {0}; z < CHUNKDEPTH; ++z)
            {
                BlockType::Type bType {blocks[x][y][z]};
                if (bType == BlockType::NOBLOCK)
                  continue;
                Mesh tmp{
                    .vertices {CUBEVERTICES.begin(), CUBEVERTICES.end()},
                    .indices {CUBEINDICES.begin(), CUBEINDICES.end()}
                };
                ModifyCubeMesh(tmp, meshCount++, bType, x, y, 0);
                cMesh.vertices.append_range(tmp.vertices);
                cMesh.indices.append_range(tmp.indices);
            }
        }
    }
    int handle {GetNewChunkHandle()};
    chunks[handle] = GPUMesh{};
    chunks[handle].UploadToGPU(device, cMesh);
    return handle;
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
