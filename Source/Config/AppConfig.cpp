#include "AppConfig.hpp"

#include <memory>
#include <algorithm>

const std::string AppConfig::Developer = "マギルゥーベルベット";
const std::string AppConfig::Name = "OTPGen";

const std::uint16_t AppConfig::VersionMajor = 0U;
const std::uint16_t AppConfig::VersionMinor = 6U;
const std::uint32_t AppConfig::PatchLevel = 1U;
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
{ return settings()->value(keyUseTheming(), true).toBool(); }

const QString AppConfig::iconColor()
{ return settings()->value(keyIconColor(), "default").toString(); }

const QString AppConfig::titleBarBackground()
{ return settings()->value(keyTitleBarBackground(), "#454545").toString(); }

const QString AppConfig::titleBarForeground()
{ return settings()->value(keyTitleBarForeground(), "#ffffff").toString(); }

const QString AppConfig::titleBarButtonBackground()
{ return settings()->value(keyTitleBarButtonBackground(), "#55555").toString(); }

const QString AppConfig::titleBarButtonForeground()
{ return settings()->value(keyTitleBarButtonForeground(), "#ffffff").toString(); }

QSettings *AppConfig::settings()
{
    static const auto init = ([&]{
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                           QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        return nullptr;
    })(); (void) init;

    static std::shared_ptr<QSettings> instance(new QSettings(
#ifdef OTPGEN_DEBUG
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings-debug.ini",
#else
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.ini",
#endif
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

void AppConfig::initDefaultSettings()
{
    if (!settingsHasKey(keyUseTheming()))
    {
        settings()->setValue(keyUseTheming(), true);
    }
    if (!settingsHasKey(keyIconColor()))
    {
        settings()->setValue(keyIconColor(), "default");
    }
    if (!settingsHasKey(keyTitleBarBackground()))
    {
        settings()->setValue(keyTitleBarBackground(), "#454545");
    }
    if (!settingsHasKey(keyTitleBarForeground()))
    {
        settings()->setValue(keyTitleBarForeground(), "#ffffff");
    }
    if (!settingsHasKey(keyTitleBarButtonBackground()))
    {
        settings()->setValue(keyTitleBarButtonBackground(), "#555555");
    }
    if (!settingsHasKey(keyTitleBarButtonForeground()))
    {
        settings()->setValue(keyTitleBarButtonForeground(), "#ffffff");
    }

    settings()->sync();
}

bool AppConfig::settingsHasKey(const QString &key)
{
    return !settings()->value(key).isNull();
}

#endif
