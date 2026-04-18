#pragma once
#include "PCH.h"

namespace InputListener
{
    class InputListener : public RE::BSTEventSink<SKSE::ModCallbackEvent>
    {
    public:
        static InputListener *GetSingleton()
        {
            static InputListener singleton;
            return &singleton;
        }

        static void Register()
        {
            auto eventSource = SKSE::GetModCallbackEventSource();
            if (eventSource)
            {
                eventSource->AddEventSink(GetSingleton());
            }
        }

        RE::BSEventNotifyControl ProcessEvent(
            const SKSE::ModCallbackEvent *a_event,
            RE::BSTEventSource<SKSE::ModCallbackEvent> *) override;
    };

    void UpdateHoldLogic();
}