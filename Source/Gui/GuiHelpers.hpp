#ifndef GUIHELPERS_HPP
#define GUIHELPERS_HPP

#include <QApplication>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QPalette>

#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QMenu>
#include <QAction>

#include <QList>

#include <WidgetHelpers/FramelessContainer.hpp>
#include <WidgetHelpers/TitleBar.hpp>

#include <memory>
#include <functional>

using QPushButtonList = QList<std::shared_ptr<QPushButton>>;

class GuiHelpers final
{
public:
    static GuiHelpers *i();

    static const QPoint centeredWindowCoords(const QWidget *parent, const QSize &useSize = QSize());
    static void centerWindow(QWidget *parent);
    static void resizeAndCenterWindow(const QSize &size, QWidget *parent);

    static const QString make_windowTitle(const QString &title);

    static std::shared_ptr<QVBoxLayout> make_vbox(int margin = 0,
                                                  int spacing = 0,
                                                  const QMargins &contentMargins = QMargins(0, 0, 0, 0));

    static std::shared_ptr<QHBoxLayout> make_hbox(int margin = 0,
                                                  int spacing = 0,
                                                  const QMargins &contentMargins = QMargins(0, 0, 0, 0));

    static std::shared_ptr<QPushButton> make_toolbtn(const QIcon &icon, const QString &tooltip,
                                                     const QObject *receiver = nullptr, const std::function<void()> &callback = []{});

    static std::shared_ptr<TitleBar> make_titlebar(QWidget *parent, const QString &windowTitle,
                                                   const QPushButtonList &leftBtns = QPushButtonList(),
                                                   const QPushButtonList &rightBtns = QPushButtonList());

    static QList<std::shared_ptr<QPushButton>> make_windowControls(const QWidget *receiver,
                                                                   bool minimize, const std::function<void()> &minimizeCallback,
                                                                   bool close, const std::function<void()> &closeCallback);

    static QList<std::shared_ptr<QPushButton>> make_tokenControls(const QWidget *receiver,
                                                                  bool add, const QString &tooltip1, const std::function<void()> &addCallback,
                                                                  bool remove, const QString &tooltip2, const std::function<void()> &removeCallback);

    static std::shared_ptr<QAction> make_menuSeparator();
    static std::shared_ptr<QAction> make_menuAction(const QString &name, const QIcon &icon,
                                                    const QObject *receiver, const std::function<void()> &callback);

    static const QPalette make_theme(const QPalette &base);
    static const QPalette make_cb_theme(const QPalette &base);

    inline const QIcon &app_icon() const             { return _app_icon; }
    inline const QIcon &tray_icon() const            { return _tray_icon; }

    inline const QIcon &close_icon() const           { return _close_icon; }
    inline const QIcon &minimize_icon() const        { return _minimize_icon; }
    inline const QIcon &maximize_icon() const        { return _maximize_icon; }
    inline const QIcon &restore_icon() const         { return _restore_icon; }

    inline const QIcon &add_icon() const             { return _add_icon; }
    inline const QIcon &remove_icon() const          { return _remove_icon; }
    inline const QIcon &delete_icon() const          { return _delete_icon; }
    inline const QIcon &save_icon() const            { return _save_icon; }
    inline const QIcon &import_icon() const          { return _import_icon; }
    inline const QIcon &export_icon() const          { return _export_icon; }
    inline const QIcon &qr_code_icon() const         { return _qr_code_icon; }
    inline const QIcon &copy_content_icon() const    { return _copy_content_icon; }
    inline const QIcon &edit_icon() const            { return _edit_icon; }
    inline const QIcon &info_icon() const            { return _info_icon; }

    static const QFont font_titleBar();

private:
    GuiHelpers();

    const QIcon loadIcon(const QString &path);

    QIcon _app_icon;
    QIcon _tray_icon;

    QIcon _close_icon;
    QIcon _minimize_icon;
    QIcon _maximize_icon;
    QIcon _restore_icon;

    QIcon _add_icon;
    QIcon _remove_icon;
    QIcon _delete_icon;
    QIcon _save_icon;
    QIcon _import_icon;
    QIcon _export_icon;
    QIcon _qr_code_icon;
    QIcon _copy_content_icon;
    QIcon _edit_icon;
    QIcon _info_icon;
};

class Scr final
{
    Scr() = delete;

    // DON'T USE FOR FONTS!!!
    // Fonts are already scaled correctly by Qt

public:
    static const QScreen *currentScreen(const QWidget *target = nullptr);
    static int scaled(int base, const QWidget *target = nullptr);
    static qreal scaledF(qreal base, const QWidget *target = nullptr);
    static const QSize scaled(const QSize &base, const QWidget *target = nullptr);
};

#endif // GUIHELPERS_HPP
