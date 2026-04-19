#include "Hooks.h"

#include "PCH.h"
#include "HookUtil.hpp"
#include "InputHandler.h"
#include "BBCState.h"
#include "AnimListener.h"

namespace Hooks
{
    namespace
    {
        struct PollInputDevicesHook
        {
            using Fn = void(RE::BSTEventSource<RE::InputEvent *> *, RE::InputEvent *const *);
            static inline std::uintptr_t func{0};

            static void thunk(RE::BSTEventSource<RE::InputEvent *> *a_dispatcher, RE::InputEvent *const *a_events)
            {
                if (!a_events)
                    return;

                RE::InputEvent *head = const_cast<RE::InputEvent *>(*a_events);

                InputHandler::ProcessAndFilter(&head);

                if (func == 0)
                    return;

                RE::InputEvent *const arr[2]{head, nullptr};
                reinterpret_cast<Fn *>(func)(a_dispatcher, arr);
            }

            static void Install()
            {
                Hook::stl::write_call<PollInputDevicesHook>(REL::RelocationID(67315, 68617),
                                                            REL::VariantOffset(0x7B, 0x7B, 0x81));
            }
        };

        struct PlayerAnimGraphProcessEventHook
        {
            using Fn = RE::BSEventNotifyControl (*)(RE::BSTEventSink<RE::BSAnimationGraphEvent> *,
                                                    const RE::BSAnimationGraphEvent *,
                                                    RE::BSTEventSource<RE::BSAnimationGraphEvent> *);
            static inline Fn _orig{nullptr};

            static RE::BSEventNotifyControl thunk(RE::BSTEventSink<RE::BSAnimationGraphEvent> *a_this,
                                                  const RE::BSAnimationGraphEvent *a_ev,
                                                  RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_src)
            {
                const auto ret = _orig ? _orig(a_this, a_ev, a_src) : RE::BSEventNotifyControl::kContinue;
                if (a_ev)
                {
                    AnimListener::HandleAnimEvent(a_ev);
                }
                return ret;
            }

            static void Install()
            {
                REL::Relocation<std::uintptr_t> vtbl{RE::VTABLE_PlayerCharacter[2]};
                const std::uintptr_t orig = vtbl.write_vfunc(1, thunk);
                _orig = reinterpret_cast<Fn>(orig);
            }
        };

        struct PlayerNotifyAnimationGraphHook
        {
            using Fn = bool (*)(RE::IAnimationGraphManagerHolder *, const RE::BSFixedString &);
            static inline Fn _orig{nullptr};

            static bool thunk(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName)
            {
                if (a_this == static_cast<RE::IAnimationGraphManagerHolder *>(RE::PlayerCharacter::GetSingleton()))
                {
                    const char *s = a_eventName.c_str();
                    const std::string_view tag = s ? s : "";

                    if(tag != "Collision_RecoilStop" && tag != "TDM_Turn_180" && tag != "turnStop")
                    BBC_DEBUG_LOG("[GraphNotify] event={}", tag);
                    if (tag == "bashStart" || tag == "bashPowerStart")
                    {
                        const bool own =
                            BBC::g_bashOwnedByMod.load(std::memory_order_relaxed);

                        BBC_DEBUG_LOG("[GraphNotify] event={} own={}", tag, own);

                        if (!own)
                        {
                            BBC_DEBUG_LOG("[GraphNotify] External bashStart detected");
                            return false;
                        }
                    }
                }

                return _orig ? _orig(a_this, a_eventName) : false;
            }

            static void Install()
            {
                REL::Relocation<std::uintptr_t> vtbl{RE::VTABLE_PlayerCharacter[3]};
                const std::uintptr_t orig = vtbl.write_vfunc(1, thunk);
                _orig = reinterpret_cast<Fn>(orig);
            }
        };
    }

    void Install_Hooks()
    {
        PollInputDevicesHook::Install();
        PlayerAnimGraphProcessEventHook::Install();
        PlayerNotifyAnimationGraphHook::Install();
    }
}