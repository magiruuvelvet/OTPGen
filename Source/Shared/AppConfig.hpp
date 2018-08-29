#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <string>
#include <cstdint>

class AppConfig final
{
public:
    static const std::string Developer;
    static const std::string Name;
    static const std::string Version;
    static const std::uint16_t VersionMajor;
    static const std::uint16_t VersionMinor;
    static const std::uint32_t PatchLevel;

private:
    AppConfig() = delete;
};

using cfg = AppConfig;

#endif // APPCONFIG_HPP
