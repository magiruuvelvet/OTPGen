#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QTimer>

#include <QShortcut>
#include <QClipboard>

#include "FramelessContainer.hpp"

#include "GuiHelpers.hpp"
#include "TokenEditor.hpp"

class MainWindow : public WidgetBase
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void minimizeToTray();

    void updateTokenList();

private:
    void addNewTokens();
    void removeSelectedTokens();

    // generated token, this->sender() QTimer
    void updateCurrentToken();

    void updateTokenValidities();

    void toggleTokenVisibility(int row, bool visible);
    void copyTokenToClipboard(int row);

    void trayShowHideCallback();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    void filterTokens(const QString &searchTerm);

private:
    std::shared_ptr<TitleBar> titleBar;

    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;

    QList<std::shared_ptr<QPushButton>> buttons;
    QList<std::shared_ptr<QPushButton>> windowControls;

    std::shared_ptr<QLineEdit> search;

    std::shared_ptr<QTimer> masterTimer;
    QList<std::shared_ptr<QTimer>> timers;

    std::shared_ptr<OTPWidget> tokenWidget;
    std::shared_ptr<TokenEditor> tokenEditor;
    std::shared_ptr<FramelessContainer> tokenEditorHelper;

    std::shared_ptr<QSystemTrayIcon> trayIcon;
    std::shared_ptr<QMenu> trayMenu;
    std::shared_ptr<QAction> trayShowHide;
    QString trayShowText;
    QString trayHideText;
    std::shared_ptr<QAction> trayDbLock;
    QString trayLock;
    QString trayUnlock;

    QClipboard *clipboard = nullptr;
};

class TokenUserData : public QObjectUserData
{
public:
    TokenUserData(const OTPToken *token)
    { this->token = token; }
    TokenUserData(const OTPToken *token, QWidget *validty, const int &row)
    { this->token = token; this->validty = validty; this->row = row; }
    const OTPToken *token = nullptr;
    QWidget *validty = nullptr;
    int row = -1;
};

#endif // MAINWINDOW_HPP
