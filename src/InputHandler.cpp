#include "InputHandler.h"
#include "BBCState.h"

static RE::ButtonEvent *MakeSyntheticBlockUpEvent()
{
    static const RE::BSFixedString userEvent("Left Attack/Block");

    return RE::ButtonEvent::Create(
        RE::INPUT_DEVICE::kMouse,
        userEvent,
        1,
        0.0f,
        0.0f);
}

inline bool IsBlockButton(const RE::ButtonEvent *btn)
{
    if (!btn)
        return false;

    const auto &ev = btn->QUserEvent();

    if (ev != "Tween Menu" && ev != "Sprint" && ev != "" && ev != "Forward" && ev != "Back" && ev != "Cancel")
    {
        if (btn->IsDown())
            BBC_DEBUG_LOG("[Input] UserEvent={} Down", ev.c_str());
        if (btn->IsUp())
            BBC_DEBUG_LOG("[Input] UserEvent={} Up", ev.c_str());
    }
    return false;
    return ev == "Left Attack/Block";
}

void InputHandler::ProcessAndFilter(RE::InputEvent **head)
{
    if (!head)
        return;

    if (BBC::ConsumeSyntheticBlockUpRequest())
    {
        if (auto *synthetic = MakeSyntheticBlockUpEvent())
        {
            BBC_DEBUG_LOG("[BBC] Injecting synthetic Left Attack/Block Up");
            synthetic->next = *head;
            *head = synthetic;
        }
    }

    if (!*head)
        return;

    RE::InputEvent *prev = nullptr;
    RE::InputEvent *cur = *head;

    while (cur)
    {
        RE::InputEvent *next = cur->next;

        if (auto *btn = cur->AsButtonEvent())
        {
            if (IsBlockButton(btn) &&
                BBC::g_suppressBlockInput.load(std::memory_order_relaxed))
            {
                BBC_DEBUG_LOG("[BBC] Filtering block input during owned bash, value={}", btn->value);

                if (prev)
                {
                    prev->next = next;
                }
                else
                {
                    *head = next;
                }

                cur = next;
                continue;
            }
        }

        prev = cur;
        cur = next;
    }
}