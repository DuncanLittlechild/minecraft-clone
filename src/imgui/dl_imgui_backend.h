#ifndef DL_TEMPLATE_IMGUIBACKEND_H
#define DL_TEMPLATE_IMGUIBACKEND_H
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"
#include "imgui_internal.h"
namespace Imgui_Backend{
    void Init(SDL_GPUDevice*, SDL_Window*);

    void ProcessEvent(SDL_Event*);

    void SetupFrame();

    void Prepare(SDL_GPUCommandBuffer*, ImGuiContext*);

    void Render(SDL_GPUCommandBuffer*, SDL_GPURenderPass*);

    void Quit();
}

#endif