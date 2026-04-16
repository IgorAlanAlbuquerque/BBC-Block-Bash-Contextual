#include "PCH.h"

#ifndef DLLEXPORT
#include "REL/Relocation.h"
#endif
#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif

namespace
{
    void InitializeLogger()
    {
        if (auto path = log::log_directory())
        {
            *path /= "ERFDestruction.log";
            auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
            auto logger = std::make_shared<spdlog::logger>("global", sink);
            spdlog::set_default_logger(logger);
            spdlog::set_level(spdlog::level::info);
            spdlog::flush_on(spdlog::level::info);
            spdlog::info("Logger iniciado.");
        }
    }

    void GlobalMessageHandler(SKSE::MessagingInterface::Message *msg)
    {
        if (!msg)
            return;
    }
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface *skse)
{
    SKSE::Init(skse);
    InitializeLogger();

    if (const auto mi = SKSE::GetMessagingInterface())
        mi->RegisterListener(GlobalMessageHandler);

    return true;
}