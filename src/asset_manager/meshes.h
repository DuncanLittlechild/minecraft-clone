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
    void Clear(SDL_GPUDevice*);

    SDL_GPUBuffer* vertices{nullptr};
    SDL_GPUBuffer* indices{nullptr};
    Uint32 indexCount{};
};

namespace CubeFace {
    enum Face {
        NORTH,
        SOUTH,
        EAST,
        WEST,
        TOP,
        BOTTOM,
        MAX_FACE,
        MIN_FACE = 0
    };
    constexpr std::array<Face, MAX_FACE> FACES {{
        NORTH,
        SOUTH,
        EAST,
        WEST,
        TOP,
        BOTTOM
    }};

    constexpr std::array<int[3], MAX_FACE> FACEDIRS {{
        {0,0,-1},
        {0,0,1},
        {1,0,0},
        {-1,0,0},
        {0,1,0},
        {0,-1,0}
    }};
};

// North = -Z, South = +Z, East = +X, West = -X
constexpr std::array<std::array<Vertex, 4>, 6> CUBEVERTICES {{
    // NORTH (-Z)
    {{
        {1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f, 0},
        {0.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0},
        {0.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0},
        {1.0f, 1.0f, 0.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f, 0},
    }},
    // SOUTH (+Z)
    {{
        {0.0f, 0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f, 0},
        {1.0f, 0.0f, 1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f, 0},
        {1.0f, 1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f, 0},
        {0.0f, 1.0f, 1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f, 0},
    }},
    // EAST (+X)
    {{
        {1.0f, 0.0f, 0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 0},
        {1.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0},
        {1.0f, 1.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 0},
        {1.0f, 0.0f, 1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f, 0},
    }},
    // WEST (-X)
    {{
        {0.0f, 0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f, 0},
        {0.0f, 1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, 0},
        {0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, 0},
        {0.0f, 0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, 0},
    }},
    // TOP (+Y)
    {{
        {0.0f, 1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0},
        {1.0f, 1.0f, 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f, 0},
        {1.0f, 1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0},
        {0.0f, 1.0f, 0.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f, 0},
    }},
    // BOTTOM (-Y)
    {{
        {0.0f, 0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f, 0},
        {1.0f, 0.0f, 0.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f, 0},
        {1.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f, 0},
        {0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f, 0},
    }}
}};

constexpr std::array<Uint16, 6> CUBEFACEINDICES {0, 1, 2, 0, 2, 3};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_MESHES_H
