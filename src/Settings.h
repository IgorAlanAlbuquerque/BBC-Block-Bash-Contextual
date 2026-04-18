#pragma once

#include "PCH.h"

namespace Settings
{
    inline bool g_requirePowerBashPerk = true;
    inline RE::FormID g_powerBashPerkID = 0x0058F61;

    void Load();
}