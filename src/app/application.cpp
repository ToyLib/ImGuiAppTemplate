#include "app/application.h"

#include "common/wake.h"
#include "sample/basic_tab.h"
#include "sample/worker_tab.h"

#include <imgui.h>

namespace
{

// ウィンドウの作り方 (アプリに合わせて変える)
RendererOptions WindowOptions()
{
    RendererOptions options;
    options.title = "ImGui App";
    options.width = 900;
    options.height = 700;
    options.fontFile = "";     // 例: "Asset/MyFont.ttf" (空ならOSの日本語フォント)
    options.fontSize = 18.0f;
    options.transparent = false;
    return options;
}

// 背景の不透明度 (透明なウィンドウのときだけ効く)
constexpr float WINDOW_OPACITY = 0.85f;

} // namespace

Application::~Application()
{
    Shutdown();
}

int Application::Run()
{
    if (!Init())
    {
        return 1;
    }

    while (ProcessEvents())
    {
        if (mRenderer.IsMinimized())
        {
            SDL_Delay(10);
            continue;
        }

        DrawFrame();

        if (mFramesToRender > 0)
        {
            --mFramesToRender;
        }
    }

    Shutdown();

    return 0;
}

bool Application::Init()
{
    if (!mRenderer.Init(WindowOptions()))
    {
        return false;
    }

    RegisterWakeEvent();
    ApplyUiColors(mColors, WINDOW_OPACITY);

    // タブの並び順 (先頭のタブを最初に開く)
    mTabs.push_back(std::make_unique<BasicTab>());
    mTabs.push_back(std::make_unique<WorkerTab>());

    return true;
}

bool Application::ProcessEvents()
{
    SDL_Event event;

    // 描くものが無い間は、イベント (入力や WakeUI) が来るまで眠る
    // 0.5 秒のタイムアウトは、入力欄のカーソルの点滅などを止めないため
    bool hasEvent = mFramesToRender > 0
        ? SDL_PollEvent(&event)
        : SDL_WaitEventTimeout(&event, 500);

    bool running = true;

    while (hasEvent)
    {
        if (mRenderer.ProcessEvent(event))
        {
            running = false;
        }

        // ImGui は操作の結果が次のフレームに出ることがあるので、数フレーム続けて描く
        mFramesToRender = 3;
        hasEvent = SDL_PollEvent(&event);
    }

    return running;
}

void Application::DrawFrame()
{
    mRenderer.BeginFrame();
    DrawMainWindow();
    mRenderer.EndFrame();
}

void Application::DrawMainWindow()
{
    // ImGui のウィンドウを、OS のウィンドウいっぱいに枠なしで出す
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin(
        "Main", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings);

    const UiContext context{mColors};

    // 背景はタイトルバーの下まで塗り、中身はその下から並べる (透明なウィンドウの macOS)
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + mRenderer.TopInset());

    // どのタブでも見せたいもの (状態の行・設定ボタンなど) は、ここでタブバーの前に描く

    if (ImGui::BeginTabBar("tabs"))
    {
        for (size_t i = 0; i < mTabs.size(); ++i)
        {
            const ImGuiTabItemFlags flags = mFirstFrame && i == 0
                ? ImGuiTabItemFlags_SetSelected
                : ImGuiTabItemFlags_None;

            if (ImGui::BeginTabItem(mTabs[i]->Label(), nullptr, flags))
            {
                mTabs[i]->Draw(context);
                ImGui::EndTabItem();
            }
        }

        mFirstFrame = false;
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Application::Shutdown()
{
    if (mShutDown)
    {
        return;
    }

    mShutDown = true;

    // ワーカースレッドを止めて、終わるのを待つ
    for (const auto& tab : mTabs)
    {
        tab->Shutdown();
    }

    // 終了に時間がかかる後始末 (外部プロセスの停止など) があるなら、
    // 先に mRenderer.Hide() でウィンドウを消してから行う
}
