#include "InputListener.h"
#include "PCH.h"
#include "BBCState.h"

extern int g_BlockActionID;

static void PerformBash()
{
    auto *player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return;

    BBC::BeginOwnedBash();
    BBC_DEBUG_LOG("[BBC] BeginOwnedBash -> bashStart");
    player->NotifyAnimationGraph("bashStart");
}

RE::BSEventNotifyControl InputListener::ProcessEvent(
    const SKSE::ModCallbackEvent *a_event,
    RE::BSTEventSource<SKSE::ModCallbackEvent> *)
{
    if (!a_event)
        return RE::BSEventNotifyControl::kContinue;

    std::string_view eventName = a_event->eventName.c_str();
    int inputID = static_cast<int>(a_event->numArg);

    if (eventName == "InputManager_ActionTriggered")
    {
        if (inputID == g_BlockActionID)
        {
            BBC_DEBUG_LOG("[Input] Double Tap Block → Bash");

            PerformBash();
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}