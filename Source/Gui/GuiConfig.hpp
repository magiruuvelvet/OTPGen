#ifndef GUICONFIG_HPP
#define GUICONFIG_HPP

#include <QString>
#include <QSettings>
#include <QSize>

class GuiConfig final
{
public:
    static const QString q(const std::string &str);

    static bool startMinimizedToTray();

    static bool useTheming();

    static const QString iconColor();

    static const QString titleBarBackground();
    static const QString titleBarForeground();
    static const QString titleBarButtonBackground();
    static const QString titleBarButtonForeground();

    static QSettings *settings();
    static const QString &path();
    static const std::string &database();

    // QSettings keys
    static const QString keyGeometryMainWindow()
    { return "UI/MainWindowGeometry"; }
    static const QString keyGeometryTokenEditor()
    { return "UI/TokenEditorGeometry"; }
    static const QString keyTokenWidgetColumns()
    { return "UI/TokenWidgetColumns"; }
    static const QString keyTokenEditWidgetColumns()
    { return "UI/TokenEditWidgetColumns"; }
    static const QString keyTokenIconsVisible()
    { return "UI/TokenIconsVisible"; }
    static const QString keyTokensVisible()
    { return "UI/TokensVisible"; }
    static const QString keyStartMinimizedToTray()
    { return "UI/MinimizeToTrayOnStart"; }
    static const QString keyUseTheming()
    { return "UI/Theming"; }
    static const QString keyIconColor()
    { return "UI/IconColor"; }
    static const QString keyTitleBarBackground()
    { return "UI/TitleBarBackground"; }
    static const QString keyTitleBarForeground()
    { return "UI/TitleBarForeground"; }
    static const QString keyTitleBarButtonBackground()
    { return "UI/TitleBarButtonBackground"; }
    static const QString keyTitleBarButtonForeground()
    { return "UI/TitleBarButtonForeground"; }

    // Settings defaults
    static const QSize defaultGeometryMainWindow()
    { return QSize(461, 490); }
    static const QSize defaultGeometryTokenEditor()
    { return QSize(960, 290); }

    static void initDefaultSettings();

private:
    GuiConfig() = delete;

    static bool settingsHasKey(const QString &key);
};

using gcfg = GuiConfig;

#endif // GUICONFIG_HPP
