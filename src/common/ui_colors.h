#pragma once

#include <imgui.h>

// 画面の色 (UiContext で各タブに渡す)
// アプリに合わせて項目を増やしてよい
struct UiColors
{
    ImVec4 background = Rgb(0x0F, 0x0F, 0x0F); // 背景 (透け具合は ApplyUiColors の opacity)
    ImVec4 text = Rgb(0xFF, 0xFF, 0xFF);       // 文字
    ImVec4 subText = Rgb(0x80, 0x80, 0x80);    // 補足の文字 (TextDisabled)
    ImVec4 accent = Rgb(0x73, 0xBF, 0xFF);     // 強調
    ImVec4 success = Rgb(0x8C, 0xE6, 0x8C);    // うまくいったこと
    ImVec4 error = Rgb(0xFF, 0x66, 0x66);      // エラー

    static constexpr ImVec4 Rgb(int r, int g, int b)
    {
        return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    }
};

// 背景・文字・補足の文字の色を ImGui のスタイルに反映する
// opacity: 背景の不透明度 (0〜1。透明なウィンドウのときだけ透けて見える)
inline void ApplyUiColors(const UiColors& colors, float opacity = 1.0f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 background = colors.background;
    background.w = opacity;
    style.Colors[ImGuiCol_WindowBg] = background;
    style.Colors[ImGuiCol_Text] = colors.text;
    style.Colors[ImGuiCol_TextDisabled] = colors.subText;
}
