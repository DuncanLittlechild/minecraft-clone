#ifndef DL_CHESS_CREATE_GPU_ASSETS_H
#define DL_CHESS_CREATE_GPU_ASSETS_H
#include <cstdint>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include "SDL3/SDL.h"
#include <string>
#include "SDL3/SDL_gpu.h"
#include "asset_manager/Vertex.h"
#include "game_api.h"

// This entire file is mostly functions which are currently just run at startup.
namespace FaceType{
    enum Type : uint32_t {
        DIRT,
        SAND,
        STONE,
        GRASS_UP,
        GRASS_SIDE,
        TYPE_COUNT,
        MIN_FACE = 0
    };
}

constexpr std::array<const char*, FaceType::TYPE_COUNT>
TEXTUREFILENAMES {{
    RESOURCES_PATH "dirt.png",
    RESOURCES_PATH "sand.png",
    RESOURCES_PATH "stone.png",
    RESOURCES_PATH "grass_up.png",
    RESOURCES_PATH "grass_side.png",
}};


// Block types which need different textures for different faces ought be recorded here
const std::unordered_map<BlockType::Type, std::unordered_map<Direction, uint32_t>>
MULTIFACEBLOCKS {
    {BlockType::GRASS, std::unordered_map<Direction, uint32_t>{
        {DIRECTION_UP, FaceType::GRASS_UP},
        {DIRECTION_NORTH, FaceType::GRASS_SIDE},
        {DIRECTION_EAST, FaceType::GRASS_SIDE},
        {DIRECTION_WEST, FaceType::GRASS_SIDE},
        {DIRECTION_SOUTH, FaceType::GRASS_SIDE},
        {DIRECTION_DOWN, FaceType::DIRT}
    }}
};

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