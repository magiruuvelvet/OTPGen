#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <memory>

#include <QLayout>

#include <QSystemTrayIcon>
#include <QEvent>
#include <QTimer>

#include <QShortcut>
#include <QClipboard>

#include <WidgetHelpers/FramelessContainer.hpp>
#include <WidgetHelpers/TitleBar.hpp>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void minimizeToTray();

private:
    void trayShowHideCallback();

signals:
    void resized();
    void closed();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;

    std::shared_ptr<QTimer> masterTimer;
    QList<std::shared_ptr<QTimer>> timers;

    std::shared_ptr<QSystemTrayIcon> trayIcon;
    std::shared_ptr<QMenu> trayMenu;
    std::shared_ptr<QAction> trayShowHide;
    QString trayShowText;
    QString trayHideText;
    std::shared_ptr<QAction> traySeparatorBeforeTokens;
    QList<std::shared_ptr<QAction>> trayTokens;

    QClipboard *clipboard = nullptr;

    std::shared_ptr<TitleBar> titleBar;
    std::shared_ptr<FramelessContainer> framelessContainer;
};

#endif // MAINWINDOW_HPP
