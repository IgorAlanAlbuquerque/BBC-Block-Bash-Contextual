#include "InputHandler.h"
#include "BBCState.h"
#include "InputListener.h"

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
    InputListener::UpdateHoldLogic();
    if (!head)
        return;

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
                BBC_DEBUG_LOG("[BBC] Filtering block input during owned bash, value={}", btn->Value());

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