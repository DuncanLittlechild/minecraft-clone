#include "AssetManager.h"
#include <array>
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "asset_manager/create_gpu_assets.h"
#include "game_api.h"
#include "meshes.h"

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

LOCAL void CreateChunkMesh(
        Block main[CHUNKX][CHUNKY][CHUNKZ], 
        Block n[CHUNKX][CHUNKY][CHUNKZ], 
        Block s[CHUNKX][CHUNKY][CHUNKZ], 
        Block e[CHUNKX][CHUNKY][CHUNKZ], 
        Block w[CHUNKX][CHUNKY][CHUNKZ], 
        int cX, int cZ,
        Mesh& cMesh
    )
{
    // Modifies an inputted cube mesh to be in the position determined by both
    // the chunk's position and the cubes position within the chunk
    auto ModifyFaceMesh {[cX, cZ](Mesh& mesh, int meshCount, Block bType, int x, int y, int z){
        std::size_t indexInc {mesh.vertices.size() * meshCount};
        for(auto& vert : mesh.vertices)
        {
            vert.x += x + cX;
            vert.y += y;
            vert.z += z + cZ;
            vert.d = static_cast<int>(bType);
        }

        for(auto& ind : mesh.indices)
        {
            ind += indexInc;
        }
    }};


    int meshCount {0};
    for(int x {0}; x < CHUNKX; ++x)
    {
        for (int y{0}; y < CHUNKY; ++y)
        {
            for (int z {0}; z < CHUNKZ; ++z)
            {
                Block bType {main[x][y][z]};
                if (bType == BlockType::NOBLOCK)
                  continue;
                // Iterate over the faces for a block
                // if the position is not off the side of the chunk, or the block at that position is solid, do not use
                // as part of the mesh
                for (auto& face : CubeFace::FACES)
                {
                    // Get the block facing that face of the current block, if it exists
                    int nPos[3] {x + CubeFace::FACEDIRS[face][0], y + CubeFace::FACEDIRS[face][1], z + CubeFace::FACEDIRS[face][2]};
                    // If the block borders another chunk, check the relevant coordinates of that chunk
                    if(nPos[0] < 0)
                    { 
                        if(w != nullptr 
                            && w[CHUNKX - 1][y][z] != BlockType::NOBLOCK)
                            continue;
                    }
                    else if(nPos[0] >= CHUNKX)
                    {
                        if (e != nullptr
                            && e[0][y][z] != BlockType::NOBLOCK)
                            continue;
                    }
                    else if(nPos[2] < 0)
                    {
                        if(n != nullptr
                            && n[x][y][CHUNKZ - 1] != BlockType::NOBLOCK)
                            continue;
                    }
                    else if(nPos[2] >= CHUNKZ)
                    {
                        if(s != nullptr
                           && s[x][y][0] != BlockType::NOBLOCK)
                            continue;
                    }
                    else if(nPos[1] >= 0 && nPos[1] < CHUNKY && main[nPos[0]][nPos[1]][nPos[2]] != BlockType::NOBLOCK )
                        continue;
                    // If the face is visible, modify it and add it to cMesh.
                    Mesh tmp{
                        .vertices {CUBEVERTICES[face].begin(), CUBEVERTICES[face].end()},
                        .indices {CUBEFACEINDICES.begin(), CUBEFACEINDICES.end()}
                    };
                    ModifyFaceMesh(tmp, meshCount++, bType, x, y, z);
                    cMesh.vertices.append_range(tmp.vertices);
                    cMesh.indices.append_range(tmp.indices);       
                }
            }
        }
    }
}

int AssetManager::CreateGPUChunk(
    SDL_GPUDevice* device,
    Block main[CHUNKX][CHUNKY][CHUNKZ], 
    Block n[CHUNKX][CHUNKY][CHUNKZ], 
    Block s[CHUNKX][CHUNKY][CHUNKZ], 
    Block e[CHUNKX][CHUNKY][CHUNKZ], 
    Block w[CHUNKX][CHUNKY][CHUNKZ], 
    int cX, int cZ
    )
{
    // Gets a unique handle
    // NOTE: too many chunks could theoretically fuck this up
    auto GetNewChunkHandle {[](){
        static int handleCount{0};
        return handleCount++;
    }};
    Mesh cMesh{};
    CreateChunkMesh(main, n, s, e, w, cX, cZ, cMesh);
    int handle {GetNewChunkHandle()};
    chunks[handle] = GPUMesh{};
    chunks[handle].UploadToGPU(device, cMesh);
    return handle;
}

void AssetManager::RecreateGPUChunk(
    SDL_GPUDevice* device,
        Block main[CHUNKX][CHUNKY][CHUNKZ], 
        Block n[CHUNKX][CHUNKY][CHUNKZ], 
        Block s[CHUNKX][CHUNKY][CHUNKZ], 
        Block e[CHUNKX][CHUNKY][CHUNKZ], 
        Block w[CHUNKX][CHUNKY][CHUNKZ], 
        int cX, int cZ,
        int handle
    )
{
    Mesh cMesh{};
    CreateChunkMesh(main, n, s, e, w, cX, cZ, cMesh);
    chunks[handle].Clear(device);
    chunks[handle].UploadToGPU(device, cMesh); 
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
