#pragma once

#include "common/ui_colors.h"

// タブを描くときに、どのタブにも渡すもの
// どのタブにも渡したいもの (フォントの大きさ・設定など) が増えたら、ここに足す
struct UiContext
{
    // 画面の色
    const UiColors& colors;
};

// メインウィンドウのタブの共通の形
// Application が持ち、選ばれているタブの Draw を毎フレーム呼ぶ
// 閉じているタブの仕事も、ワーカースレッドで動き続ける
class Tab
{
public:
    virtual ~Tab() = default;

    // タブに出す名前
    virtual const char* Label() const = 0;

    // タブの中身を描く (タブが選ばれているときだけ、毎フレーム呼ぶ)
    virtual void Draw(const UiContext& context) = 0;

    // 終了するときに呼ぶ (ワーカースレッドを止めて、終わるのを待つ)
    virtual void Shutdown() {}
};
