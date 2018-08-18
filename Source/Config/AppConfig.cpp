#include "AppConfig.hpp"

#include <algorithm>

const std::string AppConfig::Name = "OTPGen";

const std::uint16_t AppConfig::VersionMajor = 0U;
const std::uint16_t AppConfig::VersionMinor = 3U;
const std::uint32_t AppConfig::PatchLevel = 37U;
const std::string AppConfig::Version =
    std::to_string(AppConfig::VersionMajor) + '.' +
    std::to_string(AppConfig::VersionMinor) + '.' +
    std::to_string(AppConfig::PatchLevel);


#ifdef OTPGEN_GUI

const QString AppConfig::q(const std::string &str)
{
    return QString::fromUtf8(str.c_str());
}

bool AppConfig::useTheming()
{ return true; }
const QString AppConfig::titleBarBackground()
{ return "#454545"; }
const QString AppConfig::titleBarForeground()
{ return "#ffffff"; }
const QString AppConfig::titleBarButtonBackground()
{ return "#555555"; }
const QString AppConfig::titleBarButtonForeground()
{ return "#ffffff"; }

#endif
