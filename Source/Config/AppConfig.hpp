#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <string>
#include <cstdint>

#ifdef OTPGEN_GUI
#include <QString>
#endif

class AppConfig final
{
public:
    static const std::string Name;
    static const std::string Version;
    static const std::uint16_t VersionMajor;
    static const std::uint16_t VersionMinor;
    static const std::uint32_t PatchLevel;

#ifdef OTPGEN_GUI
    static const QString q(const std::string &str);

    static bool useTheming();

    static const QString titleBarBackground();
    static const QString titleBarForeground();
    static const QString titleBarButtonBackground();
    static const QString titleBarButtonForeground();
#endif

private:
    AppConfig() = delete;
};

using cfg = AppConfig;

#endif // APPCONFIG_HPP