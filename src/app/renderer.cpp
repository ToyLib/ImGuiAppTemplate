#include "app/renderer.h"

#include "common/font.h"
#include "common/ime.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#if defined(__APPLE__)
#include "common/macos_window.h"
#endif

Renderer::~Renderer()
{
    if (mBackendInitialized)
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }

    if (mImGuiCreated)
    {
        ImGui::DestroyContext();
    }

    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
    }

    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
    }

    if (mSdlInitialized)
    {
        SDL_Quit();
    }
}

bool Renderer::Init(const RendererOptions& options)
{
    mTransparent = options.transparent;

    SetImeHint();

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    mSdlInitialized = true;

    const float scale =
        SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    // 隠して作り、中央に動かしてから表示する (一瞬別の場所に見えないように)
    SDL_WindowFlags flags =
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_HIDDEN |
        SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (mTransparent)
    {
        flags |= SDL_WINDOW_TRANSPARENT;
    }

    mWindow = SDL_CreateWindow(
        options.title,
        static_cast<int>(options.width * scale),
        static_cast<int>(options.height * scale),
        flags);

    if (!mWindow)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);

    if (!mRenderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

#if defined(__APPLE__)
    if (mTransparent)
    {
        ApplyMacTitleBarStyle(mWindow);
    }
#endif

    SDL_SetRenderVSync(mRenderer, 1);
    SDL_SetWindowPosition(
        mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(mWindow);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    mImGuiCreated = true;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    LoadFont(io, options.fontFile, options.fontSize);

    ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
    ImGui_ImplSDLRenderer3_Init(mRenderer);
    mBackendInitialized = true;

    InstallImeHook();

    return true;
}

bool Renderer::ProcessEvent(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
    HandleCompositionEvent(event);

    if (event.type == SDL_EVENT_QUIT)
    {
        return true;
    }

    return event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event.window.windowID == SDL_GetWindowID(mWindow);
}

bool Renderer::IsMinimized() const
{
    return (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_MINIMIZED) != 0;
}

void Renderer::BeginFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer::EndFrame()
{
    const ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    SDL_SetRenderScale(
        mRenderer,
        io.DisplayFramebufferScale.x,
        io.DisplayFramebufferScale.y);

    // 透明なウィンドウでは透明でクリアし、ImGui の背景色だけを半透明で重ねる
    SDL_SetRenderDrawColorFloat(
        mRenderer, 0.0f, 0.0f, 0.0f, mTransparent ? 0.0f : 1.0f);
    SDL_RenderClear(mRenderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(
        ImGui::GetDrawData(), mRenderer);
    SDL_RenderPresent(mRenderer);
}

float Renderer::TopInset() const
{
#if defined(__APPLE__)
    return mTransparent ? GetMacTitleBarHeight(mWindow) : 0.0f;
#else
    return 0.0f;
#endif
}

void Renderer::Hide()
{
    if (mWindow)
    {
        SDL_HideWindow(mWindow);
    }
}
