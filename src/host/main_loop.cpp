#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include "game_api.h"
#include "core/global_consts.h"
#include "renderer/renderer.h"
#include "imgui/dl_imgui_backend.h"
#include "host/event_handler.h"



LOCAL GameMemory g_gameMemory {};
LOCAL EngineApi g_engineApi{};
LOCAL UserInput g_userInput{};

LOCAL SDL_Window* g_window{nullptr};
LOCAL Uint64 g_lastTime {0};

LOCAL constexpr int INITIALWIDTH{1280};
LOCAL constexpr int INITIALHEIGHT{720};

LOCAL double TIME_COUNTER{0.0};
LOCAL double FRAME_COUNT{0.0};


void InitEngineApi()
{
    g_engineApi.GetChunkHandle = Renderer::GetChunkHandle;
    g_engineApi.DrawChunk = Renderer::DrawChunk;
    g_engineApi.RerenderChunk = Renderer::RerenderChunk;
    g_engineApi.SyncCameraToPlayer = SyncCameraToPlayer;
}

bool InitGameMemory()
{
	g_gameMemory.gameDataSize = GetGameDataSizeFn();
    g_gameMemory.gameData = SDL_malloc(g_gameMemory.gameDataSize);
    return g_gameMemory.gameData != nullptr;
}

bool InitWindow()
{
    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    g_window = SDL_CreateWindow("Low Level Game", INITIALWIDTH, INITIALHEIGHT, SDL_WINDOW_RESIZABLE);

    if(g_window == nullptr) {
        return false;
    }

    SDL_ShowWindow(g_window);
    SDL_RaiseWindow(g_window);

    // Allow fp[s style mouse movement
    //SDL_SetWindowRelativeMouseMode(g_window, true);
    return true;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!InitGameMemory())
    {
        SDL_Log("Couldnt allocate memory: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if(!InitWindow())
    {
        SDL_Log("Failed to initialise window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    // Multiple possible reasons for this function to fail,
    // so the log is created within the function
    if(!Renderer::Init(g_window, INITIALWIDTH, INITIALHEIGHT))
    {
        return SDL_APP_FAILURE;
    }
    InitEngineApi();

    Imgui_Backend::Init(Renderer::GetGPURenderingDevice(), g_window);

    GameInitFn(g_gameMemory, g_engineApi);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
    Imgui_Backend::ProcessEvent(event);
    return HandleEvent(event, g_userInput);
}

SDL_AppResult SDL_AppIterate(void* appstate){
    Uint64 currentTime {SDL_GetPerformanceCounter()};
    double deltaTime {(currentTime - g_lastTime) / (double)SDL_GetPerformanceFrequency()};
    g_lastTime = currentTime;
    TIME_COUNTER += deltaTime;
    FRAME_COUNT += 1.0;
    if(TIME_COUNTER >= 1.0)
    {
        FRAME_COUNT /= TIME_COUNTER;
        char fpsCount[5];
        sprintf(fpsCount, "%.2f", FRAME_COUNT);
        SDL_SetWindowTitle(g_window, fpsCount);
        FRAME_COUNT = 0.0;
        TIME_COUNTER = 0.0;
    }

    Imgui_Backend::SetupFrame();

    // RTry to acquire the swapchain
    // If it can't be acquired, nothing will be renderered, and theres no point in 
    // pushing sprite data to the queue
    bool skipRender{false};

    RendererSetupResult rendererSetup {Renderer::TryAcquireSwapchain()};
    switch(rendererSetup){
        case(RendererSetupResult::FAILURE):{
            return SDL_APP_FAILURE;
        } break;
        case(RendererSetupResult::SWAPCHAIN_ACQUIRED):{
            skipRender = false;
        } break;
        case(RendererSetupResult::SWAPCHAIN_NOT_ACQUIRED):{
            skipRender = true;
        } break;
    };

    GameUpdateFn(g_gameMemory, g_engineApi, g_userInput, deltaTime, skipRender);
    ResetInput(g_userInput);
    if(!skipRender)
        Renderer::RenderGame(g_gameMemory.imGuiContext);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
    Imgui_Backend::Quit();
    Renderer::Quit();
    SDL_DestroyWindow(g_window);
}