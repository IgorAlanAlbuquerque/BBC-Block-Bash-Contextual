#pragma once
#include <atomic>

namespace BBC
{
    inline std::atomic_bool g_bashOwnedByMod{false};
    inline std::atomic_bool g_suppressBlockInput{false};
    inline std::atomic_bool g_bashAnimFinished{false};

    inline void BeginOwnedBash()
    {
        g_bashOwnedByMod.store(true, std::memory_order_relaxed);
        g_suppressBlockInput.store(true, std::memory_order_relaxed);
        g_bashAnimFinished.store(false, std::memory_order_relaxed);
    }

    inline void EndOwnedBashNow()
    {
        g_suppressBlockInput.store(false, std::memory_order_relaxed);
        g_bashOwnedByMod.store(false, std::memory_order_relaxed);
        g_bashAnimFinished.store(true, std::memory_order_relaxed);
    }
}