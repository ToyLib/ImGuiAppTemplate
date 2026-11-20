#include "sample/basic_tab.h"

#include "common/ime.h"
#include "common/widgets.h"

#include <imgui.h>
#include <imgui_stdlib.h>

void BasicTab::Draw(const UiContext& context)
{
    ui::WrappedDisabled(
        "ボタン・入力欄・チェックボックスの見本です。"
        "状態はタブのメンバー (mName など) に持ち、毎フレームそれをもとに描きます。");
    ImGui::Spacing();

    ImGui::SeparatorText("入力欄");
    ImGui::Checkbox("入力できるようにする", &mEditable);

    {
        // スコープを抜けると EndDisabled が呼ばれる
        ui::Disabled disabled(!mEditable);
        ImGui::InputTextWithHint("名前", "日本語も入力できます", &mName);
    }

    // IME で変換中 (未確定) の文字を、入力カーソルの位置に重ねて描く (入力欄の後に呼ぶ)
    DrawComposition();

    if (mName.empty())
    {
        ImGui::TextDisabled("名前を入力すると、ここにあいさつが出ます");
    }
    else
    {
        ui::WrappedColored(context.colors.accent, "こんにちは、" + mName + " さん");
    }

    ImGui::SeparatorText("ボタン");

    if (ImGui::Button("押す"))
    {
        ++mClicks;
    }

    ImGui::SameLine();

    // 押せないときは薄く表示し、false を返す
    if (ui::Button("リセット", mClicks > 0))
    {
        mClicks = 0;
    }

    ImGui::SameLine();
    ImGui::Text("%d 回押しました", mClicks);

    ImGui::SeparatorText("スライダー");
    ImGui::SliderFloat("音量", &mVolume, 0.0f, 1.0f, "%.2f");

    {
        // 3 つ目の引数が false なら色を変えない
        ui::StyleColor warn(ImGuiCol_Text, context.colors.error, mVolume > 0.8f);
        ImGui::Text("%s", mVolume > 0.8f ? "大きすぎます" : "ちょうどよい音量です");
    }
}
