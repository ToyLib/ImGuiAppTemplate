#pragma once

// ワーカースレッドから UI スレッドを起こす
//
// Application はイベントが無い間 SDL_WaitEventTimeout で眠っている
// ワーカースレッドが状態を書き換えたら WakeUI を呼ぶと、すぐに次のフレームが描かれる

// 独自のイベント番号を 1 つ確保する (SDL_Init の後、ワーカーを始める前に 1 回呼ぶ)
void RegisterWakeEvent();

// UI を起こす (どのスレッドから呼んでもよい)
void WakeUI();
