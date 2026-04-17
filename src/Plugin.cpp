#include "PCH.h"

#include "InputListener.h"
#include "InputManagerAPI.h"
#include "Hooks.h"

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

int g_BlockActionID = -1;

void InitializeLogger()
{
    if (auto path = SKSE::log::log_directory())
    {
        *path /= "BlockBashContextual.log";
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
        auto logger = std::make_shared<spdlog::logger>("global", sink);

        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);

        spdlog::info("BlockBashContextual started.");
    }
}

void SetupInputManagerAPI()
{
    using namespace InputManagerAPI;

    auto *api = RequestAPIDirect();

    if (!api)
    {
        spdlog::error("[API] Failed to obtain InputManager API!");
        return;
    }

    spdlog::info("[API] InputManager API obtained successfully!");

    constexpr const char *actionName = "BBC_BlockBash";
    bool createdNow = false;

    const size_t count = api->GetInputCount(0);

    for (size_t i = 0; i < count; ++i)
    {
        const char *name = api->GetInputName(0, static_cast<int>(i));
        if (name && std::strcmp(name, actionName) == 0)
        {
            g_BlockActionID = static_cast<int>(i);
            spdlog::info("[API] Existing action found ID={}", g_BlockActionID);
            break;
        }
    }

    if (g_BlockActionID == -1)
    {
        g_BlockActionID = api->CreateInput(0, actionName);
        createdNow = (g_BlockActionID >= 0);
        spdlog::info("[API] Action created ID={}", g_BlockActionID);
    }

    if (g_BlockActionID < 0)
    {
        spdlog::error("[API] Invalid action ID.");
        return;
    }

    api->UpdateListener(
        0,
        g_BlockActionID,
        "BlockBashContextual",
        "Double Tap Block -> Bash",
        true);

    auto info = api->GetActionInfo(g_BlockActionID);

    if (createdNow || !info.isValid)
    {
        info.id = g_BlockActionID;
        info.name = actionName;

        info.pcMainKey = 257;
        info.pcMainAction = 1;
        info.pcMainTapCount = 2;

        info.pcModifierKey = 0;
        info.pcModAction = 0;
        info.pcModTapCount = 1;

        info.gamepadMainKey = 275;
        info.gamepadMainAction = 1;
        info.gamepadMainTapCount = 2;

        info.gamepadModifierKey = 0;
        info.gamepadModAction = 0;
        info.gamepadModTapCount = 1;

        info.gamepadGestureStick = 0;

        info.useCustomTimings = true;
        info.holdDuration = 0.20f;
        info.tapWindow = 0.35f;
        info.isValid = true;

        if (api->UpdateActionMapping(g_BlockActionID, info))
        {
            spdlog::info("[API] Default mapping applied.");
        }
        else
        {
            spdlog::error("[API] Failed to apply default mapping.");
        }
    }
    else
    {
        spdlog::info("[API] Keeping existing user mapping.");
    }
}

void GlobalMessageHandler(SKSE::MessagingInterface::Message *msg)
{
    if (!msg)
        return;

    switch (msg->type)
    {
    case SKSE::MessagingInterface::kInputLoaded:
    {
        spdlog::info("[MSG] kInputLoaded");
        SetupInputManagerAPI();
        break;
    }
    case SKSE::MessagingInterface::kDataLoaded:
        spdlog::info("[MSG] kDataLoaded");
        Hooks::Install_Hooks();
        spdlog::info("[MSG] Hooks installed.");
        InputListener::Register();
        spdlog::info("[MSG] Listener registered.");
        break;
    }
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface *skse)
{
    SKSE::Init(skse);
    InitializeLogger();
    if (const auto mi = SKSE::GetMessagingInterface())
    {
        mi->RegisterListener(GlobalMessageHandler);
    }

    return true;
}