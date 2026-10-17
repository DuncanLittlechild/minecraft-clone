#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_MESHES_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_MESHES_H
#include <vector>
#include <array>
#include "asset_manager/Vertex.h"
#include "SDL3/SDL.h"

struct Mesh{    
    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
};

struct GPUMesh{
    bool UploadToGPU(SDL_GPUDevice*, const Mesh&);

    SDL_GPUBuffer* vertices{nullptr};
    SDL_GPUBuffer* indices{nullptr};
    Uint32 indexCount{};
};

constexpr std::array<Vertex, 24> CUBEVERTICES {{
    // Front (+Z) - red
    {0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0},
    { 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0},
    { 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0},
    {0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0},

    // Back (-Z) - green
    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0},
    {0.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0},
    { 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0},

    // Left (-X) - blue
    {0.0f, 0.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0},
    {0.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0},
    {0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0},
    {0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0},

    // Right (+X) - yellow
    { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0},
    { 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0},
    { 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0},
    { 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0},

    // Bottom (-Y) - magenta
    {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0},
    { 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0},
    { 1.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0},
    {0.0f, 0.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0},

    // Top (+Y) - cyan
    {0.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0},
    { 1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0},
    { 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0},
    {0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0},
}};

constexpr std::array<uint32_t, 36> CUBEINDICES {{
     0,  1,  2,   0,  2,  3,   // Front
     4,  5,  6,   4,  6,  7,   // Back
     8,  9, 10,   8, 10, 11,   // Left
    12, 13, 14,  12, 14, 15,   // Right
    16, 17, 18,  16, 18, 19,   // Bottom
    20, 21, 22,  20, 22, 23,   // Top
}};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_MESHES_H
