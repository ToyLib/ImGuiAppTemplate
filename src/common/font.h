#pragma once

#include <imgui.h>

#include <string>

// 日本語フォントを読み込む
// 環境変数 IMGUI_APP_FONT → fontFile → OSのフォント の順に探し、
// 足りない文字はOSのフォントで補う
// fontFile が空ならOSのフォントだけを使う
// fontFile が相対パスなら実行ファイルのフォルダーから探す (パスはUTF-8。例: "Asset/MyFont.ttf")
void LoadFont(ImGuiIO& io, const std::string& fontFile, float fontSize);
