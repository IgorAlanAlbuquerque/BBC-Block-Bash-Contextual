#pragma once
#include <atomic>

namespace BBC
{
    inline std::atomic_bool g_bashOwnedByMod{false};
    inline std::atomic_bool g_suppressBlockInput{false};
    inline std::atomic_bool g_waitBlockPhysicalRelease{false};
    inline std::atomic_bool g_blockPhysicalHeld{false};
    inline std::atomic_bool g_bashAnimFinished{false};
    inline std::atomic_bool g_pendingSyntheticBlockUp{false};

    inline void BeginOwnedBash()
    {
        g_bashOwnedByMod.store(true, std::memory_order_relaxed);
        g_suppressBlockInput.store(true, std::memory_order_relaxed);
        g_waitBlockPhysicalRelease.store(true, std::memory_order_relaxed);
        g_bashAnimFinished.store(false, std::memory_order_relaxed);
        g_blockPhysicalHeld.store(false, std::memory_order_relaxed);
    }

    inline void RequestSyntheticBlockUp()
    {
        g_pendingSyntheticBlockUp.store(true, std::memory_order_relaxed);
    }

    inline bool ConsumeSyntheticBlockUpRequest()
    {
        return g_pendingSyntheticBlockUp.exchange(false, std::memory_order_relaxed);
    }

    inline void EndOwnedBashNow()
    {
        g_blockPhysicalHeld.store(false, std::memory_order_relaxed);
        g_bashAnimFinished.store(true, std::memory_order_relaxed);
        g_suppressBlockInput.store(false, std::memory_order_relaxed);
        g_waitBlockPhysicalRelease.store(false, std::memory_order_relaxed);
        g_bashOwnedByMod.store(false, std::memory_order_relaxed);
    }
}