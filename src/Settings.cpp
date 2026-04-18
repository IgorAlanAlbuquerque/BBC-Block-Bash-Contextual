#include "Settings.h"
#include <SimpleIni.h>

void Settings::Load()
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(L"Data/SKSE/Plugins/BBC.ini");

    g_requirePowerBashPerk = ini.GetBoolValue("BBC", "bRequirePowerBashPerk", true);

    const char *perkIDStr = ini.GetValue("BBC", "uPowerBashPerkID", "0x0058F61");
    g_powerBashPerkID = static_cast<RE::FormID>(std::stoul(perkIDStr, nullptr, 16));
}