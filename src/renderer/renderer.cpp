#include "renderer.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
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
LOCAL std::array<SpriteRenderData, 1024> g_drawDataQueue{};
std::size_t g_drawDataQueueEnd{0};

LOCAL Uint32 g_w{};
LOCAL Uint32 g_h{};

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
    g_drawDataQueueEnd = 0; // No data is erased, but just mark all indices as erasable
}

bool Renderer::Init(SDL_Window* window)
{
    g_window = window;

    // Set flags for the shader formats which this program can use
    SDL_GPUShaderFormat gpuFlags {SDL_GPU_SHADERFORMAT_SPIRV|SDL_GPU_SHADERFORMAT_DXIL|SDL_GPU_SHADERFORMAT_MSL};

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

void Renderer::DrawSprite(const SpriteRenderData& spriteData)
{
    g_drawDataQueue[g_drawDataQueueEnd++] = spriteData;
}

// Iterates over draw data queue
void RenderQueue(SDL_GPURenderPass* renderPass)
{
    // Everythings a cube, so only need to push gpu mesh atm
    std::array cubeVertexBuffers {
        SDL_GPUBufferBinding{
            .buffer = g_assetManager.cubeMesh.vertices,
            .offset = 0
        }
    };
    SDL_GPUBufferBinding cubeIndexBuffer {
        .buffer = g_assetManager.cubeMesh.indices,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(renderPass, 0, cubeVertexBuffers.data(), cubeVertexBuffers.size());
    SDL_BindGPUIndexBuffer(renderPass, &cubeIndexBuffer, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    for (auto i {0uz}; i < g_drawDataQueueEnd; ++i)
    {
        SpriteRenderData& spr {g_drawDataQueue[i]};
        Matrix4x4 movMat {Matrix4x4_CreateTranslation(spr.x, spr.y, spr.z)};
        SDL_PushGPUVertexUniformData(g_commandBuffer, 0, &movMat, sizeof(Matrix4x4));
        SDL_DrawGPUIndexedPrimitives(renderPass, g_assetManager.cubeMesh.indexCount, 1, 0, 0, 0);
    }
}

void Renderer::RenderGame(ImGuiContext* context)
{
    Imgui_Backend::Prepare(g_commandBuffer, context);
    /* PREPARE INFO STRUCTS*/
    SDL_GPUColorTargetInfo targetInfo {
        .texture = g_swapchainTexture,
        .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };
    /* START RENDER PASS*/\
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(g_commandBuffer, &targetInfo, 1, nullptr);
    /* BIND PIPELINES AND DRAW */

    SDL_GPUDepthStencilTargetInfo depthInfo{
        .texture = g_assetManager.depthTexture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_DONT_CARE
    };

    /* BIND PIPELINES AND DRAW */
    SDL_BindGPUGraphicsPipeline(renderPass, g_assetManager.mainPipeline);


    DL_VertexPerLoop vertexPerLoopUB{
        .cameraVPMat = g_camera.vpMat,
    };
    SDL_PushGPUVertexUniformData(g_commandBuffer, 1, &vertexPerLoopUB, sizeof(DL_VertexPerLoop));

    RenderQueue(renderPass);

    Imgui_Backend::Render(g_commandBuffer, renderPass);
    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(g_commandBuffer);

    ResetGlobals();
}


SDL_GPUDevice* Renderer::GetGPURenderingDevice() { return g_renderDevice; }