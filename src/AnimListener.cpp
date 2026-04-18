#include "AnimListener.h"
#include "BBCState.h"

void AnimListener::HandleAnimEvent(const RE::BSAnimationGraphEvent *ev)
{
    if (!ev || !ev->holder)
        return;

    if (auto *actor = ev->holder->As<RE::Actor>();
        actor != RE::PlayerCharacter::GetSingleton())
    {
        return;
    }

    const std::string_view tag{ev->tag.c_str(), ev->tag.size()};
    if (!BBC::g_bashOwnedByMod.load(std::memory_order_relaxed))
        return;

    if (tag == "bashExit" || tag == "bashStop")
    {
        const bool alreadyFinished =
            BBC::g_bashAnimFinished.load(std::memory_order_relaxed);

        if (!alreadyFinished)
        {
            BBC::EndOwnedBashNow();
            BBC_DEBUG_LOG("[BBC] MarkBashAnimFinished after {}", tag);
        }
    }
}