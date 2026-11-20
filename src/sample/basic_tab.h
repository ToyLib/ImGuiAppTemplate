#pragma once

#include "common/tab.h"

#include <string>

// 見本のタブ (1): ボタン・入力欄・チェックボックスだけの簡単なタブ
// 画面の状態はタブのメンバーに持ち、毎フレームそれをもとに描く (イミディエイトモード)
class BasicTab : public Tab
{
public:
    const char* Label() const override { return "基本"; }
    void Draw(const UiContext& context) override;

private:
    std::string mName;
    bool mEditable = true;
    int mClicks = 0;
    float mVolume = 0.5f;
};
