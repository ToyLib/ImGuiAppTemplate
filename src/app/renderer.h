#pragma once

#include <SDL3/SDL.h>

#include <string>

// ウィンドウの作り方
struct RendererOptions
{
    const char* title = "ImGui App";

    // 100% 表示のときの大きさ (画面の拡大率を掛けて作る)
    int width = 900;
    int height = 700;

    // フォントファイル (空ならOSの日本語フォント) と大きさ (ピクセル)。LoadFont を参照
    std::string fontFile;
    float fontSize = 18.0f;

    // ウィンドウの背景を透明にする (ImGui の背景の不透明度だけ透けて見える)
    // macOS では、タイトルバーも本文と一体にする
    bool transparent = false;
};

// ウィンドウと描画の土台: SDL のウィンドウとレンダラー、ImGui、フォント、IME
// 画面の中身 (タブ) は知らない。Application が BeginFrame と EndFrame の間で描く
class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // SDL・ウィンドウ・レンダラー・ImGui を用意して、ウィンドウを表示する
    // 失敗したらログに書いて false (途中まで作ったものはデストラクタで片付ける)
    bool Init(const RendererOptions& options);

    // ImGui と IME にイベントを渡す。ウィンドウを閉じる操作なら true
    bool ProcessEvent(const SDL_Event& event);

    bool IsMinimized() const;

    // 1 フレームの描画の始まりと終わり (間で ImGui の関数を呼ぶ)
    void BeginFrame();
    void EndFrame();

    // 画面上端に重なっているタイトルバーの高さ (透明なウィンドウの macOS 以外は 0)
    float TopInset() const;

    // 終了処理に時間がかかるときに、先にウィンドウを隠す
    void Hide();

private:
    SDL_Window* mWindow = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    bool mTransparent = false;

    // 後片付けで、作ったものだけを壊すための印
    bool mSdlInitialized = false;
    bool mImGuiCreated = false;
    bool mBackendInitialized = false;
};
