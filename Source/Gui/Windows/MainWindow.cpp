#include "MainWindow.hpp"
#include "GuiConfig.hpp"
#include "GuiHelpers.hpp"

#include <TokenDatabase.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QRootWidget(parent)
{
    // initial window state
    GuiHelpers::resizeAndCenterWindow(Scr::scaled(gcfg::defaultGeometryMainWindow(), this), this);

    data.titleBar = GuiHelpers::make_titlebar(this, "");

    data.vbox->addWidget(data.titleBar.get());
    data.vbox->addSpacerItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->setLayout(data.vbox.get());

    // create system tray icon
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = std::make_shared<QSystemTrayIcon>(this);
        trayIcon->setIcon(GuiHelpers::i()->tray_icon());

        trayMenu = std::make_shared<QMenu>();
        trayIcon->setContextMenu(trayMenu.get());

        trayShowHide = std::make_shared<QAction>();
        trayShowText = QObject::tr("Show");
        trayHideText = QObject::tr("Hide");
        trayShowHide->setText(trayHideText);
        QObject::connect(trayShowHide.get(), &QAction::triggered, this, &MainWindow::trayShowHideCallback);
        trayMenu->addAction(trayShowHide.get());

        trayMenu->addSeparator();

        traySeparatorBeforeTokens = std::make_shared<QAction>();
        traySeparatorBeforeTokens->setSeparator(true);
        trayMenu->addAction(traySeparatorBeforeTokens.get());
        traySeparatorBeforeTokens->setVisible(false);

        trayMenu->addAction(QString(QObject::tr("Quit %1")).arg(qApp->applicationDisplayName()), this, [&]{
            qApp->quit();
        }, QKeySequence("Ctrl+Q"));

        QObject::connect(trayIcon.get(), &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger)
            {
                trayShowHideCallback();
            }
        });

        trayIcon->show();

        if (gcfg::startMinimizedToTray())
        {
            trayShowHide->setText(trayShowText);
        }
    }

    // Quit Application
    auto ctrl_q = new QShortcut(QKeySequence("Ctrl+Q"), this);
    QObject::connect(ctrl_q, &QShortcut::activated, this, [&]{
        this->close();
    });

    // Initialize Clipboard
    clipboard = QGuiApplication::clipboard();

    // Restore UI state
    const auto _geometry = saveGeometry();
    restoreGeometry(gcfg::settings()->value(gcfg::keyGeometryMainWindow(), _geometry).toByteArray());
}

MainWindow::~MainWindow()
{
    clipboard = nullptr;
}

void MainWindow::minimizeToTray()
{
    // minimize to tray when available, otherwise minimize normally
    if (trayIcon)
    {
        trayShowHide->setText(trayShowText);
        this->hide();
    }
    else
    {
        this->showMinimized();
    }
}

void MainWindow::trayShowHideCallback()
{
    if (this->isVisible())
    {
        this->hide();
        trayShowHide->setText(trayShowText);
    }
    else
    {
        this->show();
        this->activateWindow();
        trayShowHide->setText(trayHideText);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    // Set system tray icon visible text
    if (trayShowHide)
    {
        trayShowHide->setText(trayHideText);
    }

    event->accept();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save UI state
    gcfg::settings()->setValue(gcfg::keyGeometryMainWindow(), saveGeometry());

    // Emit closed event
    emit closed();
    event->accept();
}
