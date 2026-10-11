#ifndef DL_TEMPLATE_MESHES_H
#define DL_TEMPLATE_MESHES_H
#include "SDL3/SDL.h"
#include <array>
#include <vector>
#include "SDL3/SDL_gpu.h"
#include "renderer/Vertex.h"

struct Mesh{    
    std::vector<Vertex> vertices{};
    std::vector<uint16_t> indices{};
};

struct GPUMesh{
    bool UploadToGPU(SDL_GPUDevice* device, const Mesh& mesh);

    SDL_GPUBuffer* vertices{nullptr};
    SDL_GPUBuffer* indices{nullptr};
    Uint32 indexCount{};
};

constexpr std::array<Vertex, 24> cubeColourVertices {{
    // Front (+Z) - red
    {-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f},
    { 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f},
    { 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f},
    {-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f},

    // Back (-Z) - green
    { 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    { 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f},

    // Left (-X) - blue
    {-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f},

    // Right (+X) - yellow
    { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    { 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    { 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f},
    { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f},

    // Bottom (-Y) - magenta
    {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    { 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    { 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f},
    {-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f},

    // Top (+Y) - cyan
    {-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f},
    { 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f},
    { 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
    {-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
}};

constexpr std::array<uint16_t, 36> cubeIndices {{
     0,  1,  2,   0,  2,  3,   // Front
     4,  5,  6,   4,  6,  7,   // Back
     8,  9, 10,   8, 10, 11,   // Left
    12, 13, 14,  12, 14, 15,   // Right
    16, 17, 18,  16, 18, 19,   // Bottom
    20, 21, 22,  20, 22, 23,   // Top
}};

struct Material{
    SDL_FColor albedo {};
    SDL_FColor specular{};
    float shininess{};
};

struct AssetManager {
    GPUMesh cubeMesh{};
    SDL_GPUTexture* textureAtlas{nullptr};
    SDL_GPUTexture* depthTexture {nullptr};
    SDL_GPUGraphicsPipeline* mainPipeline{nullptr};

    void Load(SDL_GPUDevice*, SDL_Window*);
    void RecreateDepthTexture(SDL_GPUDevice*, SDL_Window*);
};


#endif