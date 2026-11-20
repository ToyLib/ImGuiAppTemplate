#include "common/wake.h"

#include <SDL3/SDL.h>

#include <atomic>

namespace
{

std::atomic<Uint32> g_wakeEvent{0};

} // namespace

void RegisterWakeEvent()
{
    g_wakeEvent = SDL_RegisterEvents(1);
}

void WakeUI()
{
    const Uint32 type = g_wakeEvent;

    if (type == 0)
    {
        return;
    }

    SDL_Event event{};
    event.type = type;
    SDL_PushEvent(&event);
}
