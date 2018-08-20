#ifndef GUIHELPERS_HPP
#define GUIHELPERS_HPP

#include <QApplication>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QPalette>

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QAction>

#include <QList>

#include "WidgetBase.hpp"
#include "TitleBar.hpp"

#include <memory>
#include <functional>

class GuiHelpers final
{
public:
    static GuiHelpers *i();

    static const QPoint centeredWindowCoords(const QWidget *parent);
    static void centerWindow(QWidget *parent);

    static const QString make_windowTitle(const QString &title);

    static std::shared_ptr<QVBoxLayout> make_vbox(int margin = 0,
                                                  int spacing = 0,
                                                  const QMargins &contentMargins = QMargins(0, 0, 0, 0));

    static std::shared_ptr<QHBoxLayout> make_hbox(int margin = 0,
                                                  int spacing = 0,
                                                  const QMargins &contentMargins = QMargins(0, 0, 0, 0));

    static std::shared_ptr<QPushButton> make_toolbtn(const QIcon &icon, const QString &tooltip,
                                                     const QObject *receiver = nullptr, const std::function<void()> &callback = []{});

    static std::shared_ptr<TitleBar> make_titlebar(const QString &windowTitle,
                                                   const QList<std::shared_ptr<QPushButton>> &leftBtns = QList<std::shared_ptr<QPushButton>>(),
                                                   const QList<std::shared_ptr<QPushButton>> &rightBtns = QList<std::shared_ptr<QPushButton>>());

    static QList<std::shared_ptr<QPushButton>> make_windowControls(const WidgetBase *receiver,
                                                                   bool minimize, const std::function<void()> &minimizeCallback,
                                                                   bool maximizeRestore, const std::function<void()> &maximizeRestoreCallback,
                                                                   bool close, const std::function<void()> &closeCallback);

    static QList<std::shared_ptr<QPushButton>> make_tokenControls(const WidgetBase *receiver,
                                                                  bool add, const QString &tooltip1, const std::function<void()> &addCallback,
                                                                  bool remove, const QString &tooltip2, const std::function<void()> &removeCallback);

    static std::shared_ptr<QAction> make_menuSeparator();
    static std::shared_ptr<QAction> make_importAction(const QString &name, const QIcon &icon,
                                                      const QObject *receiver, const std::function<void()> &callback);

    static void default_minimizeCallback(WidgetBase *receiver);
    static void default_maximizeRestoreCallback(WidgetBase *receiver);
    static void default_closeCallback(WidgetBase *receiver);

    static const QPalette make_theme(const QPalette &base);
    static const QPalette make_cb_theme(const QPalette &base);

    const QIcon app_icon;
    const QIcon tray_icon;

    const QIcon close_icon;
    const QIcon minimize_icon;
    const QIcon maximize_icon;
    const QIcon restore_icon;

    const QIcon add_icon;
    const QIcon remove_icon;
    const QIcon save_icon;
    const QIcon import_icon;
    const QIcon qr_code_icon;
    const QIcon copy_content_icon;

private:
    GuiHelpers();
};

class AppIcon : public QObjectUserData
{
public:
    AppIcon() :
        icon(GuiHelpers::i()->app_icon)
    { }
    QIcon icon;
};

#endif // GUIHELPERS_HPP
