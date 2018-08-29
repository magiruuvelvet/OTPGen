#include "GuiConfig.hpp"

#include <QStandardPaths>
#include <QSystemTrayIcon>

#include <memory>

const QString GuiConfig::q(const std::string &str)
{
    return QString::fromUtf8(str.c_str());
}

bool GuiConfig::startMinimizedToTray()
{
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        return settings()->value(keyStartMinimizedToTray(), false).toBool();
    }
    else
    {
        return false;
    }
}

bool GuiConfig::useTheming()
{ return settings()->value(keyUseTheming(), true).toBool(); }

const QString GuiConfig::iconColor()
{ return settings()->value(keyIconColor(), "default").toString(); }

const QString GuiConfig::titleBarBackground()
{ return settings()->value(keyTitleBarBackground(), "#454545").toString(); }

const QString GuiConfig::titleBarForeground()
{ return settings()->value(keyTitleBarForeground(), "#ffffff").toString(); }

const QString GuiConfig::titleBarButtonBackground()
{ return settings()->value(keyTitleBarButtonBackground(), "#55555").toString(); }

const QString GuiConfig::titleBarButtonForeground()
{ return settings()->value(keyTitleBarButtonForeground(), "#ffffff").toString(); }

QSettings *GuiConfig::settings()
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

const QString &GuiConfig::path()
{
    static const auto location = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return location;
}

const std::string &GuiConfig::database()
{
#ifdef OTPGEN_DEBUG
    static const std::string db = (path() + "/database.debug").toUtf8().constData();
#else
    static const std::string db = (path() + "/database").toUtf8().constData();
#endif
    return db;
}

void GuiConfig::initDefaultSettings()
{
    if (!settingsHasKey(keyStartMinimizedToTray()))
    {
        settings()->setValue(keyStartMinimizedToTray(), false);
    }
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

bool GuiConfig::settingsHasKey(const QString &key)
{
    return !settings()->value(key).isNull();
}
