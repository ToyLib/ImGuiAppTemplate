# ImGuiAppTemplate

SDL3 + Dear ImGui で作るデスクトップアプリの雛形です。
ウィンドウと ImGui の初期化、省電力のメインループ、タブ、ワーカースレッドから画面を更新する仕組み、日本語の入力 (IME) とフォントまでを用意してあります。
新しいアプリは、この雛形をコピーしてタブを書くところから始められます。

| 入っているもの | 内容 |
| --- | --- |
| `Renderer` | SDL のウィンドウとレンダラー、ImGui の初期化と後始末、高解像度ディスプレイ、透明なウィンドウ (オプション) |
| `Application` | タブの一覧、イベントが無い間は眠るメインループ、終了の順番 |
| `Tab` / `UiContext` | タブの共通の形と、どのタブにも渡すもの (色) |
| `ui::` | ImGui の Begin/End・Push/Pop の組を、スコープを抜けると閉じるようにしたラッパー |
| `WakeUI` | ワーカースレッドから、眠っている UI を起こす |
| IME・フォント | 変換中 (未確定) の文字の表示と、OS の日本語フォントの読み込み |
| 見本のタブ | 「基本」(ボタン・入力欄・チェックボックス) と「ワーカー」(スレッドで処理して進み具合を出す) |

クラスの分け方とその理由は [doc/class-design.md](doc/class-design.md) にまとめています。

## 必要なもの

- C++20 対応のコンパイラ
- CMake 3.20 以上
- SDL3
- Dear ImGui (v1.92.9b。初回の configure で CMake の FetchContent が GitHub から取得するので、ネット接続が必要)

| OS | 入れ方 |
| --- | --- |
| macOS | `brew install cmake sdl3` |
| Ubuntu | `sudo apt install build-essential cmake libsdl3-dev fonts-noto-cjk` |
| Arch Linux | `sudo pacman -S --needed base-devel cmake sdl3 noto-fonts-cjk` |
| Windows | Visual Studio と vcpkg。環境変数 `VCPKG_ROOT` に vcpkg のフォルダーを設定し、`& "$env:VCPKG_ROOT\vcpkg.exe" install sdl3 --triplet x64-windows` |

日本語を表示するには OS の日本語フォントが必要です (Windows は游ゴシックかメイリオ、macOS はヒラギノ、Linux は Noto Sans CJK)。

## ビルドと実行

macOS / Linux:

```bash
cmake --preset debug
```

```bash
cmake --build --preset debug
```

```bash
./build/ImGuiApp
```

Release は `--preset release` で、`build-release/` に作られます。

Windows (通常の PowerShell。開発者用のシェルではなく):

```powershell
cmake --preset windows
cmake --build --preset windows-debug
.\build\Debug\ImGuiApp.exe
```

VS Code では、C/C++ と CMake Tools の拡張機能を入れて開くと、Ctrl+Shift+B (macOS は Cmd+Shift+B) でビルド、F5 でデバッグできます。

## 新しいアプリを始める

1. このリポジトリをコピーします (GitHub の template repository にしてあれば「Use this template」)。
2. 名前を変えます。
   - `CMakeLists.txt` の `set(APP_NAME ImGuiApp)` (実行ファイルの名前)
   - `.vscode/launch.json` と `.vscode/tasks.json` の `ImGuiApp`
   - `src/app/application.cpp` の `WindowOptions()` の `options.title` (ウィンドウのタイトル)
3. ウィンドウの作り方を決めます (`WindowOptions()`)。
   - `fontFile`: フォントを同梱するときは `Asset/` に置いて `"Asset/MyFont.ttf"` のように指定します。空なら OS の日本語フォントを使います。環境変数 `IMGUI_APP_FONT` でも指定できます。
   - `transparent`: `true` にするとウィンドウの背景が透け、透け具合は `WINDOW_OPACITY` で決まります。macOS ではタイトルバーも本文と一体になります。
4. タブを書きます (次の節)。見本のタブ (`src/sample/`) は、要らなくなったら消して、`Application::Init` と `CMakeLists.txt` からも外します。
5. `LICENSE` の著作権者を、自分のアプリに合わせて直します。

## タブを足す

```cpp
// src/notes/notes_tab.h
#include "common/tab.h"

class NotesTab : public Tab
{
public:
    const char* Label() const override { return "メモ"; }
    void Draw(const UiContext& context) override;

private:
    std::string mText;   // 画面の状態はタブのメンバーに持つ
};
```

1. `Tab` を継承したクラスを作り、`Label()` と `Draw()` を書きます。`Draw()` は、タブが選ばれている間、毎フレーム呼ばれます。
2. ワーカースレッドを使うなら `Shutdown()` も書き、スレッドを止めて終わるのを待ちます (見本: `src/sample/worker_tab.cpp`)。ワーカーは状態を書き換えたら `WakeUI()` を呼びます。
3. `src/app/application.cpp` の `Application::Init` で `mTabs.push_back(std::make_unique<NotesTab>());` します。並べた順にタブが出て、先頭のタブが最初に選ばれます。
4. `CMakeLists.txt` の `add_executable` にソースを足します。

入力欄を描いたら、IME で変換中の文字を出すために `DrawComposition()` (`common/ime.h`) を呼んでください。

## ファイル構成

| ファイル | 内容 |
| --- | --- |
| `src/main.cpp` | 入口 (`Application` を動かすだけ) |
| `src/app/application.{h,cpp}` | アプリ本体: タブの一覧、メインループ、起動と終了の手順 |
| `src/app/renderer.{h,cpp}` | ウィンドウと描画の土台: SDL と Dear ImGui の初期化と描画 |
| `src/common/tab.h` | タブの共通の形 (`Tab`) と、どのタブにも渡すもの (`UiContext`) |
| `src/common/ui_colors.h` | 画面の色 (`UiColors`) と、ImGui のスタイルへの反映 |
| `src/common/widgets.{h,cpp}` | ImGui の薄いラッパー (`ui::`) |
| `src/common/wake.{h,cpp}` | ワーカースレッドから UI を起こす (`WakeUI`) |
| `src/common/ime.{h,cpp}` | IME の変換中の文字列の表示 |
| `src/common/font.{h,cpp}` | 日本語フォントの読み込み |
| `src/common/macos_window.{h,mm}` | macOS のタイトルバーの設定 (透明なウィンドウのとき) |
| `src/sample/` | 見本のタブ (`BasicTab`、`WorkerTab`) |
| `Asset/` | 実行ファイルの隣にコピーするもの (フォントなど) |
| `doc/class-design.md` | クラス設計 |
