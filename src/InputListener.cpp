#include "InputListener.h"
#include "PCH.h"
#include "BBCState.h"
#include "Settings.h"

extern int g_BlockActionID;
namespace
{
    int g_tapCount = 0;
    uint32_t g_lastTapTime = 0;
    uint32_t g_secondPressTime = 0;
    bool g_waitingSecondRelease = false;
    bool g_secondPressActive = false;
    bool g_powerTriggered = false;

    constexpr uint32_t kDoubleTapWindow = 350;
    constexpr uint32_t kHoldThreshold = 250;

    void PerformBash()
    {
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (!player)
            return;

        BBC::BeginOwnedBash();
        BBC_DEBUG_LOG("[BBC] BeginOwnedBash -> bashStart");
        player->NotifyAnimationGraph("bashStart");
    }

    void PerformPowerBash()
    {
        auto *player = RE::PlayerCharacter::GetSingleton();
        if (!player)
            return;

        BBC::BeginOwnedBash();
        BBC_DEBUG_LOG("[BBC] BeginOwnedBash -> bashPowerStart");
        player->NotifyAnimationGraph("bashPowerStart");
    }
}

RE::BSEventNotifyControl InputListener::InputListener::ProcessEvent(
    const SKSE::ModCallbackEvent *a_event,
    RE::BSTEventSource<SKSE::ModCallbackEvent> *)
{
    if (!a_event)
        return RE::BSEventNotifyControl::kContinue;

    std::string_view eventName = a_event->eventName.c_str();
    auto inputID = static_cast<int>(a_event->numArg);

    if (eventName == "InputManager_ActionTriggered")
    {
        if (inputID == g_BlockActionID)
        {
            uint32_t now = RE::GetDurationOfApplicationRunTime();

            if ((now - g_lastTapTime) <= kDoubleTapWindow)
            {
                g_tapCount++;
            }
            else
            {
                g_tapCount = 1;
            }
            g_lastTapTime = now;
            BBC_DEBUG_LOG("[Input] Tap count={}", g_tapCount);
            if (g_tapCount == 2)
            {
                if (Settings::g_requirePowerBashPerk)
                {
                    BBC_DEBUG_LOG("[Input] Checking for Power Bash perk (ID={:08X})", Settings::g_powerBashPerkID);
                    const auto powerBashPerk = RE::TESForm::LookupByID<RE::BGSPerk>(Settings::g_powerBashPerkID);
                    BBC_DEBUG_LOG("[Input] powerBashPerk lookup = {}", powerBashPerk ? "OK" : "nullptr");
                    if (auto const *player = RE::PlayerCharacter::GetSingleton(); !player || !powerBashPerk || !player->HasPerk(powerBashPerk))
                    {
                        BBC_DEBUG_LOG("[Input] No Power Bash perk → Immediate Normal Bash");
                        PerformBash();
                        g_tapCount = 0;
                        return RE::BSEventNotifyControl::kContinue;
                    }
                }

                g_waitingSecondRelease = true;
                g_secondPressActive = true;
                g_secondPressTime = now;
                g_powerTriggered = false;
                BBC_DEBUG_LOG("[Input] Second press started, waiting for hold or release");
            }
        }
    }
    else if (eventName == "InputManager_ActionReleased")
    {
        if (inputID == g_BlockActionID)
        {
            uint32_t now = RE::GetDurationOfApplicationRunTime();
            BBC_DEBUG_LOG("[Input] Block Released");

            if (g_waitingSecondRelease && g_secondPressActive)
            {
                if (uint32_t heldTime = now - g_secondPressTime; heldTime < kHoldThreshold)
                {
                    BBC_DEBUG_LOG("[Input] Double Tap → Normal Bash");
                    PerformBash();
                }

                g_tapCount = 0;
                g_waitingSecondRelease = false;
                g_secondPressActive = false;
            }
        }
    }

    return RE::BSEventNotifyControl::kContinue;
}

void InputListener::UpdateHoldLogic()
{
    if (!g_waitingSecondRelease || !g_secondPressActive)
        return;

    uint32_t now = RE::GetDurationOfApplicationRunTime();
    uint32_t heldTime = now - g_secondPressTime;

    if (heldTime >= kHoldThreshold && !g_powerTriggered)
    {
        BBC_DEBUG_LOG("[Input] Hold threshold reached → Power Bash");
        PerformPowerBash();
        g_powerTriggered = true;
        g_tapCount = 0;
        g_waitingSecondRelease = false;
        g_secondPressActive = false;
    }
}