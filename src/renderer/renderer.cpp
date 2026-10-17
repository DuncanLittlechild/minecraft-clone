#include "renderer.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_hints.h"
#include "asset_manager/meshes.h"
#include "core/Matrix4x4.h"
#include "core/global_consts.h"
#include "game_api.h"
#include "imgui/dl_imgui_backend.h"
#include "imgui_internal.h"
#include <array>
#include "asset_manager/AssetManager.h"
#include "renderer/Camera3d.h"

// Exist from init to quit
LOCAL SDL_GPUDevice* g_renderDevice{nullptr};
LOCAL SDL_Window* g_window{nullptr};
LOCAL AssetManager g_assetManager{};
LOCAL Camera3d g_camera{};

// Refreshed every frame
LOCAL SDL_GPUCommandBuffer* g_commandBuffer{nullptr};
LOCAL SDL_GPUTexture* g_swapchainTexture{nullptr};
LOCAL std::array<int, 1024> g_drawDataQueue{};
std::size_t g_drawDataQueueEnd{0};

LOCAL Uint32 g_w{};
LOCAL Uint32 g_h{};

LOCAL GPUMesh g_CubeMesh {};

void CreateCubeMesh()
{
    Mesh cMesh {
        .vertices {CUBEVERTICES.begin(), CUBEVERTICES.end()},
        .indices {CUBEINDICES.begin(), CUBEINDICES.end()}
    };
    g_CubeMesh.UploadToGPU(g_renderDevice, cMesh);
}

struct DL_VertexPerLoop{
    Matrix4x4 cameraVPMat{};
};

struct DL_VertexPerObject{
    Matrix4x4 moveMat{};
};

struct DL_LightingFragPerLoop{
    Vector3 cameraPos{};
    float gpuPadding{};
    Vector3 lightPos{};
};

struct DL_LightingFragPerObject{
    Vector3 trueColour{};
    float gpuPadding{};
    Vector3 specularColour{};
    float shinyness{};
};

void ResetGlobals()
{
    g_swapchainTexture = nullptr;
    g_commandBuffer = nullptr;
    g_drawDataQueueEnd = 0; // No data is erased, but just mark all indices as erasable
}

bool Renderer::Init(SDL_Window* window, float w, float h)
{
    g_window = window;

    // Set flags for the shader formats which this program can use
    SDL_GPUShaderFormat gpuFlags {SDL_GPU_SHADERFORMAT_SPIRV|SDL_GPU_SHADERFORMAT_DXIL|SDL_GPU_SHADERFORMAT_MSL};

    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");

    // Create a truct which interfaces with a GPU device which meets the criteria establishefd in the flags
    g_renderDevice = SDL_CreateGPUDevice(gpuFlags, true, NULL);
    if (g_renderDevice == nullptr){
        SDL_Log("Coudn't create GPU Device: %s", SDL_GetError());
        return false;
    }

    // Links the GPU device to a specific window
    if(!SDL_ClaimWindowForGPUDevice(g_renderDevice, g_window)){
        SDL_Log("Couldn't claim window for GPU device: %s\n", SDL_GetError());
        return false;
    }

    g_assetManager.Load(g_renderDevice, g_window);

    g_camera.Init(w, h);

    return true;
}

void Renderer::Quit()
{
    SDL_ReleaseWindowFromGPUDevice(g_renderDevice, g_window);
    SDL_DestroyGPUDevice(g_renderDevice);
}


RendererSetupResult Renderer::TryAcquireSwapchain()
{
    g_commandBuffer = SDL_AcquireGPUCommandBuffer(g_renderDevice);
    if (g_commandBuffer == nullptr) {
        SDL_Log("Could not acquire command buffer from gpu: %s", SDL_GetError());
        return RendererSetupResult::FAILURE;
    }

    Uint32 w, h;

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(g_commandBuffer, g_window, &g_swapchainTexture, &w, &h))
    {
        SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
        return RendererSetupResult::FAILURE;
    }

    if(w != g_w || h != g_h)
    {
        g_w = w;
        g_h = h;
        g_camera.UpdateAspectRatio(w, h);
        g_assetManager.RecreateDepthTexture(g_renderDevice, g_window);
    }

    if(g_swapchainTexture == nullptr)
        return RendererSetupResult::SWAPCHAIN_NOT_ACQUIRED;
    else
        return RendererSetupResult::SWAPCHAIN_ACQUIRED;
}

void Renderer::DrawChunk(int handle)
{
    g_drawDataQueue[g_drawDataQueueEnd++] = handle;
}

void Renderer::UpdateCamera(float xMov, float zMov, float xCamMov, float yCamMov)
{
    g_camera.UpdateCam(xMov, zMov, xCamMov, yCamMov);
}

// Iterates over draw data queue
void RenderQueue(SDL_GPURenderPass* renderPass)
{
    for (auto i {0uz}; i < g_drawDataQueueEnd; ++i)
    {
        // Everythings a cube, so only need to push gpu mesh atm
        std::array cubeVertexBuffers {
            SDL_GPUBufferBinding{
                .buffer = g_assetManager.chunks[i].vertices,
                .offset = 0
            }
        };
        SDL_GPUBufferBinding cubeIndexBuffer {
            .buffer = g_assetManager.chunks[i].indices,
            .offset = 0
        };
        SDL_BindGPUVertexBuffers(renderPass, 0, cubeVertexBuffers.data(), cubeVertexBuffers.size());
        SDL_BindGPUIndexBuffer(renderPass, &cubeIndexBuffer, SDL_GPU_INDEXELEMENTSIZE_32BIT);
        SDL_DrawGPUIndexedPrimitives(renderPass, g_assetManager.chunks[i].indexCount, 1, 0, 0, 0);
    }
}

void Renderer::RenderGame(ImGuiContext* context)
{
    Imgui_Backend::Prepare(g_commandBuffer, context);
    /* PREPARE INFO STRUCTS*/
    SDL_GPUColorTargetInfo targetInfo {
        .texture = g_swapchainTexture,
        .clear_color = SDL_FColor{.0f, .0f, .0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    SDL_GPUDepthStencilTargetInfo depthInfo{
        .texture = g_assetManager.depthTexture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_DONT_CARE
    };
    /* START RENDER PASS*/
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(g_commandBuffer, &targetInfo, 1, &depthInfo);
    /* BIND PIPELINES AND DRAW */

    /* BIND PIPELINES AND DRAW */
    SDL_BindGPUGraphicsPipeline(renderPass, g_assetManager.mainPipeline);

    DL_VertexPerLoop vertexPerLoopUB{
        .cameraVPMat = g_camera.vpMat,
    };
    SDL_PushGPUVertexUniformData(g_commandBuffer, 0, &vertexPerLoopUB, sizeof(DL_VertexPerLoop));

    SDL_GPUTextureSamplerBinding samplerBinding {
        .texture = g_assetManager.textureArray,
        .sampler = g_assetManager.sampler
    };
    SDL_BindGPUFragmentSamplers(renderPass, 0, &samplerBinding, 1);

    RenderQueue(renderPass);

    Imgui_Backend::Render(g_commandBuffer, renderPass);
    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(g_commandBuffer);

    ResetGlobals();
}


SDL_GPUDevice* Renderer::GetGPURenderingDevice() { return g_renderDevice; }

int Renderer::GetChunkHandle(BlockType::Type blocks[CHUNKWIDTH][CHUNKHEIGHT][CHUNKDEPTH], int cX, int cY)
{
    return g_assetManager.CreateGPUChunk(g_renderDevice, blocks, cX, cY);
}