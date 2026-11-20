#include "sample/worker_tab.h"

#include "common/wake.h"
#include "common/widgets.h"

#include <imgui.h>

#include <cfloat>
#include <chrono>

void WorkerTab::Start()
{
    // 前のワーカーは終わっている (running が false) ので、すぐ戻る
    if (mWorker.joinable())
    {
        mWorker.join();
    }

    const int total = mSteps;

    {
        std::lock_guard lock(mMutex);
        mShared.running = true;
        mShared.done = 0;
        mShared.total = total;
        mShared.result.clear();
    }

    mCancel = false;

    mWorker = std::thread([this, total] {
        int done = 0;

        for (; done < total && !mCancel; ++done)
        {
            // 時間のかかる処理の代わり
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            {
                std::lock_guard lock(mMutex);
                mShared.done = done + 1;
            }

            WakeUI();
        }

        {
            std::lock_guard lock(mMutex);
            mShared.running = false;
            mShared.result = mCancel
                ? "中断しました (" + std::to_string(done) + " / " + std::to_string(total) + ")"
                : "終わりました";
        }

        WakeUI();
    });
}

void WorkerTab::Shutdown()
{
    mCancel = true;

    if (mWorker.joinable())
    {
        mWorker.join();
    }
}

void WorkerTab::Draw(const UiContext& context)
{
    // 描く前に状態をコピーして、すぐロックを外す
    // (ロックを持ったまま描くと、その間ワーカーが書き込めない)
    Shared shared;
    {
        std::lock_guard lock(mMutex);
        shared = mShared;
    }

    ui::WrappedDisabled(
        "ワーカースレッドで時間のかかる処理をし、進み具合を画面に出す見本です。"
        "ワーカーは状態を書き換えるたびに WakeUI() を呼び、眠っている UI を起こします。");
    ImGui::Spacing();

    {
        ui::Disabled disabled(shared.running);
        ImGui::SliderInt("ステップ数", &mSteps, 5, 100);
    }

    if (ui::Button("開始", !shared.running))
    {
        Start();
    }

    ImGui::SameLine();

    if (ui::Button("中断", shared.running))
    {
        mCancel = true;
    }

    const float fraction = shared.total > 0
        ? static_cast<float>(shared.done) / static_cast<float>(shared.total)
        : 0.0f;
    const std::string overlay =
        std::to_string(shared.done) + " / " + std::to_string(shared.total);

    ImGui::ProgressBar(fraction, ImVec2(-FLT_MIN, 0.0f), overlay.c_str());

    if (shared.running)
    {
        ImGui::TextDisabled("処理しています…");
    }
    else if (!shared.result.empty())
    {
        ImGui::TextColored(
            mCancel ? context.colors.error : context.colors.success,
            "%s", shared.result.c_str());
    }
}
