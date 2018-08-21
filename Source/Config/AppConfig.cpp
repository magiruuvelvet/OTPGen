#include "AppConfig.hpp"

#include <memory>
#include <algorithm>

const std::string AppConfig::Developer = "マギルゥーベルベット";
const std::string AppConfig::Name = "OTPGen";

const std::uint16_t AppConfig::VersionMajor = 0U;
const std::uint16_t AppConfig::VersionMinor = 3U;
const std::uint32_t AppConfig::PatchLevel = 37U;
const std::string AppConfig::Version =
    std::to_string(AppConfig::VersionMajor) + '.' +
    std::to_string(AppConfig::VersionMinor) + '.' +
    std::to_string(AppConfig::PatchLevel);


#ifdef OTPGEN_GUI

#include <QStandardPaths>

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

QSettings *AppConfig::settings()
{
    static const auto init = ([&]{
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                           QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        return nullptr;
    })(); (void) init;

    static std::shared_ptr<QSettings> instance(new QSettings(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.ini",
        QSettings::IniFormat));
    return instance.get();
}

const QString &AppConfig::path()
{
    static const auto location = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return location;
}

const std::string &AppConfig::database()
{
#ifdef OTPGEN_DEBUG
    static const std::string db = (path() + "/database.debug").toUtf8().constData();
#else
    static const std::string db = (path() + "/database").toUtf8().constData();
#endif
    return db;
}

#endif
