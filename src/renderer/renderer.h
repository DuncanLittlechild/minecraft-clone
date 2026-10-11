#ifndef DL_TEMPLATE_RENDERER_H
#define DL_TEMPLATE_RENDERER_H
#include "SDL3/SDL_gpu.h"
#include "game_api.h"
#include "imgui_internal.h"
/*
Renderer.cpp is effectively a TU as an object. It contains all the state needed for rendering as global objects,
meaning the game layer can make calls directly into it without having to pass data backwards and forwards.
*/

// Used for tryacquireswapchaintexture - see below
enum class RendererSetupResult {
    SWAPCHAIN_ACQUIRED,
    SWAPCHAIN_NOT_ACQUIRED,
    FAILURE,
    RESULTCOUNT
};

namespace Renderer {
    // Initialises all state held within the renderer, and stores a pointer to the window
    bool Init(SDL_Window*, float w, float h);

    // Destroys resources owned by the renderer layer
    // note that this does not inclduethe window, which is owned by the main loop
    void Quit();

    // Checks to see if the swapchain can be acquired. This is because, if no
    // swapchain canbe acquire, there is no point in the game layer uploading
    // sprites to the sprite queue, and this must be done in advance to flag if
    // this is the case. Also updates depth texture and camera aspect ratio on 
    // window resize
    RendererSetupResult TryAcquireSwapchain();
    // Adds sprite render data to the renderer queue
    void DrawSprite(const SpriteRenderData&);

    void UpdateCamera(float, float, float, float);

    // Sets up the pipelines and renders the sprite queue
    void RenderGame(ImGuiContext*);

    SDL_GPUDevice* GetGPURenderingDevice();
}

#endif