#pragma once

#include "common/tab.h"

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

// 見本のタブ (2): 時間のかかる処理をワーカースレッドで行い、進み具合を画面に出す
//
// - ワーカーは状態をロックして書き換え、WakeUI() で UI を起こす
// - UI スレッドは、ロックを持つ時間を短くするため、描く前に状態をコピーする
// - 中断は cancel のフラグで頼み、Shutdown でスレッドの終了を待つ
class WorkerTab : public Tab
{
public:
    const char* Label() const override { return "ワーカー"; }
    void Draw(const UiContext& context) override;
    void Shutdown() override;

private:
    // ワーカーを始める (前のワーカーが終わっていれば、待ってから)
    void Start();

    // ワーカーと UI スレッドで共有する状態 (mutex で守る)
    struct Shared
    {
        bool running = false;
        int done = 0;
        int total = 0;
        std::string result;
    };

    std::mutex mMutex;
    Shared mShared;

    // どのスレッドからでもロックなしで読み書きできるもの
    std::atomic<bool> mCancel = false;

    std::thread mWorker;

    // UI スレッドだけが触るもの
    int mSteps = 30;
};
