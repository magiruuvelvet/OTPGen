#include "AppConfig.hpp"

#include <algorithm>

const std::string AppConfig::Developer = "マギルゥーベルベット";
const std::string AppConfig::Name = "OTPGen";

const std::uint16_t AppConfig::VersionMajor = 0U;
const std::uint16_t AppConfig::VersionMinor = 7U;
const std::uint32_t AppConfig::PatchLevel = 5U;
const std::string AppConfig::Version =
    std::to_string(AppConfig::VersionMajor) + '.' +
    std::to_string(AppConfig::VersionMinor) + '.' +
    std::to_string(AppConfig::PatchLevel);
