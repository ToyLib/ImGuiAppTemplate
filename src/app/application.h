#pragma once

#include "app/renderer.h"
#include "common/tab.h"
#include "common/ui_colors.h"

#include <memory>
#include <vector>

// アプリ本体: タブと、タブが共有するものを持ち、メインループを回す
// ウィンドウと ImGui の細かいことは Renderer に任せる
class Application
{
public:
    Application() = default;
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // 初期化してメインループを回す。終わったら終了コード (0 = 正常)
    int Run();

private:
    bool Init();

    // イベントを処理する。ウィンドウが閉じられたら false
    bool ProcessEvents();

    void DrawFrame();

    // 画面全体: タブバーと、選ばれているタブ
    void DrawMainWindow();

    // タブのワーカーを止めて待つ
    void Shutdown();

    Renderer mRenderer;

    // タブが共有するもの (タブが参照するなら、mTabs より前に宣言する。後に壊れるように)
    UiColors mColors;

    std::vector<std::unique_ptr<Tab>> mTabs;

    // 最初のフレームだけ、先頭のタブを選ぶ
    bool mFirstFrame = true;

    // 入力後しばらくは毎フレーム描画し、それ以外はイベント待ちで省電力
    int mFramesToRender = 3;

    bool mShutDown = false;
};
