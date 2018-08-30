#include "MainWindow.hpp"
#include "GuiConfig.hpp"

#include <TokenDatabase.hpp>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : WidgetBase(parent)
{
    // initial window size
    this->resize(gcfg::defaultGeometryMainWindow());

    GuiHelpers::centerWindow(this);

    buttons = GuiHelpers::make_tokenControls(this,
        true, "Add tokens", [&]{ addNewTokens(); },
        true, "Remove selected tokens", [&]{ removeSelectedTokens(); }
    );
    buttons.append(GuiHelpers::make_toolbtn(GuiHelpers::i()->edit_icon(), "Edit selected tokens", this, [&]{
        editSelectedTokens();
    }));
    buttons.append(GuiHelpers::make_toolbtn(GuiHelpers::i()->export_icon(), "Export all tokens", this, [&]{
        exportAllTokens();
    }));

    windowControls += GuiHelpers::make_toolbtn(GuiHelpers::i()->info_icon(), "Info", this, [&]{
        // TODO: implement about dialog
        QMessageBox::information(this, "Not Implemented", "This feature is not implemented yet.");
    });
    windowControls += GuiHelpers::make_windowControls(this,
        true, [&]{ minimizeToTray(); },
        true, [&]{ GuiHelpers::default_maximizeRestoreCallback(this); },
        true, [&]{ GuiHelpers::default_closeCallback(this); }
    );

    titleBar = GuiHelpers::make_titlebar("", buttons, windowControls);
    vbox->addWidget(titleBar.get());

    search = std::make_shared<QLineEdit>();
    search->setPlaceholderText("Search tokens...");
    search->setContentsMargins(3,0,3,0);
    search->setFrame(false);
    search->setAutoFillBackground(true);
    search->setFixedHeight(30);
    QObject::connect(search.get(), &QLineEdit::textChanged, this, &MainWindow::filterTokens);
    innerVBox->addWidget(search.get());

    tokenWidget = std::make_shared<OTPWidget>();
    innerVBox->addWidget(tokenWidget.get());

    vbox->addLayout(innerVBox.get());
    this->setLayout(vbox.get());

    // create master timer
    masterTimer = std::make_shared<QTimer>();
    masterTimer->setInterval(1000); // 1 second
    QObject::connect(masterTimer.get(), &QTimer::timeout, this, &MainWindow::updateTokenValidities);
    masterTimer->start();

    // create system tray icon
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = std::make_shared<QSystemTrayIcon>(this);
        trayIcon->setIcon(GuiHelpers::i()->tray_icon());

        trayMenu = std::make_shared<QMenu>();
        trayIcon->setContextMenu(trayMenu.get());

        trayShowHide = std::make_shared<QAction>();
        trayShowText = "Show";
        trayHideText = "Hide";
        trayShowHide->setText(trayHideText);
        QObject::connect(trayShowHide.get(), &QAction::triggered, this, &MainWindow::trayShowHideCallback);
        trayMenu->addAction(trayShowHide.get());

        trayMenu->addSeparator();

        traySeparatorBeforeTokens = std::make_shared<QAction>();
        traySeparatorBeforeTokens->setSeparator(true);
        trayMenu->addAction(traySeparatorBeforeTokens.get());
        traySeparatorBeforeTokens->setVisible(false);

        trayMenu->addAction(QString("Quit %1").arg(qApp->applicationDisplayName()), this, [&]{
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

    // load stored tokens
    this->updateTokenList();

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
    const auto columns = gcfg::settings()->value(gcfg::keyTokenWidgetColumns());
    if (!columns.isNull())
    {
        QSequentialIterable iterable = columns.value<QSequentialIterable>();
        int c = 0;
        for (auto&& col : iterable)
        {
            tokenWidget->tokens()->setColumnWidth(c, col.toInt());
            ++c;
        }
    }
    auto tokenVisibility = gcfg::settings()->value(gcfg::keyTokensVisible(), tokenWidget->tokenVisibilityAction()->isChecked()).toBool();
    tokenWidget->tokenVisibilityAction()->setChecked(!tokenVisibility);
    tokenWidget->tokenVisibilityAction()->trigger();

    auto tokenIconVisibility = gcfg::settings()->value(gcfg::keyTokenIconsVisible(), tokenWidget->tokenIconVisibilityAction()->isChecked()).toBool();
    tokenWidget->tokenIconVisibilityAction()->setChecked(!tokenIconVisibility);
    tokenWidget->tokenIconVisibilityAction()->trigger();
}

MainWindow::~MainWindow()
{
    buttons.clear();
    windowControls.clear();
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

void MainWindow::updateTokenList()
{
    auto tokens = tokenWidget->tokens();
    tokens->setUpdating(true);

    // remove all active timers
    timers.clear();

    for (auto i = 0; i < tokens->rowCount(); i++)
    {
        for (auto j = 0; j < tokens->columnCount(); j++)
        {
            delete tokens->cellWidget(i, j);
        }
    }

    tokens->clearContents();
    tokens->setRowCount(0);
    tokens->setInvalidRows({});

    if (trayMenu)
    {
        trayTokens.clear();
        traySeparatorBeforeTokens->setVisible(false);
    }

    // reload tokens from store

    QList<int> invalidRows;
    for (auto&& token : TokenStore::i()->tokens())
    {
        const auto row = tokens->rowCount();

        // row user data:
        //  - Show

        const auto valid = token->valid();

        tokens->insertRow(row);
        tokens->setCellWidget(row, 0, OTPWidget::make_showToggle(row, this,
            [&](bool c){ toggleTokenVisibility(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row, c); },
            [&]        { copyTokenToClipboard(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row); }));
        tokens->setCellWidget(row, 1, OTPWidget::make_typeDisplay(token.get()));
        tokens->setCellWidget(row, 2, OTPWidget::make_labelDisplay(
                                          token->icon(),
                                          QString::fromUtf8(token->label().c_str())));
        tokens->setCellWidget(row, 3, OTPWidget::make_tokenGenDisplay(token->period(), token->type()));
        qobject_cast<QWidget*>(tokens->cellWidget(row, 3))->findChild<QProgressBar*>()->setVisible(false);
        tokens->cellWidget(row, 3)->setUserData(0, new TokenUserData(token.get()));
        qobject_cast<QWidget*>(tokens->cellWidget(row, 3))->findChild<QLineEdit*>()->setVisible(false);
        qobject_cast<QWidget*>(tokens->cellWidget(row, 3))->findChild<QProgressBar*>()->setVisible(false);

        if (trayMenu)
        {
            std::shared_ptr<QAction> tokenTray = std::make_shared<QAction>();
            tokenTray->setText(QString::fromUtf8(token->label().c_str()));
            if (!token->icon().empty())
            {
                QPixmap pixmap;
                const auto status = pixmap.loadFromData(reinterpret_cast<const unsigned char*>(token->icon().data()), static_cast<uint>(token->icon().size()));
                if (status)
                {
                    tokenTray->setIcon(pixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }
            QObject::connect(tokenTray.get(), &QAction::triggered, this, [&]{
                copyTokenToClipboard(qobject_cast<QAction*>(this->sender())->text());
            });
            trayTokens.append(tokenTray);
        }

        if (!valid)
        {
            tokens->tokenShow(row)->setChecked(true);
            tokens->tokenSecret(row)->setText("INVALID");
            tokens->tokenSecretTimeout(row)->setVisible(false);
            tokens->tokenControlWidget(row)->setDisabled(true);
            tokens->tokenSecretWidget(row)->setDisabled(true);
            invalidRows << row;
            if (trayMenu)
            {
                trayTokens.last()->setDisabled(true);
            }
        }
        else
        {
            // setup timers
            if (token->type() != OTPToken::HOTP)
            {
                timers.append(std::make_shared<QTimer>());
                timers.last()->setInterval(static_cast<int>(1000 * token->period()));
                timers.last()->setUserData(0, new TokenUserData(token.get(), tokens->tokenSecretTimeout(row), row));
                QObject::connect(timers.last().get(), &QTimer::timeout, this, &MainWindow::updateCurrentToken);

                auto token_validity = token->remainingTokenValidity();

                // set initial interval of timer to remaining token validity
                // interval is updated to the real period after the first timeout
                timers.last()->setInterval(1000 * token_validity);
                tokens->tokenSecret(row)->setText(
                    token->generateToken().c_str()
                );
                timers.last()->start();
                tokens->tokenSecretTimeout(row)->setValue(
                    timers.last()->remainingTime() / 1000
                );
            }
        }
    }
    tokens->setInvalidRows(invalidRows);
    tokens->setUpdating(false);

    if (trayMenu && !trayTokens.isEmpty())
    {
        QList<QAction*> _tokenTrayPtrs;
        for (auto&& tray : trayTokens)
        {
            _tokenTrayPtrs.append(tray.get());
        }
        traySeparatorBeforeTokens->setVisible(true);
        trayMenu->insertActions(traySeparatorBeforeTokens.get(), _tokenTrayPtrs);
    }
}

void MainWindow::addNewTokens()
{
    if (tokenEditor)
    {
        tokenEditor->show();
        tokenEditor->activateWindow();
    }
    else
    {
        tokenEditor = std::make_shared<TokenEditor>();
        tokenEditorHelper = std::make_shared<FramelessContainer>(tokenEditor.get());
        QObject::connect(tokenEditor.get(), &WidgetBase::closed, this, [&]{
            tokenEditor.reset();
            tokenEditorHelper.reset();
        });
        QObject::connect(tokenEditor.get(), &TokenEditor::tokensSaved, this, &MainWindow::updateTokenList);
        tokenEditor->show();
        tokenEditor->activateWindow();
    }
}

void MainWindow::removeSelectedTokens()
{
    auto res = QMessageBox::question(this, "Delete Tokens",
                                     "Are you sure you want to delete all selected tokens? This action can't be undone!");
    if (res != QMessageBox::Yes)
    {
        return;
    }

    for (auto i = 0; i < tokenWidget->tokens()->rowCount(); i++)
    {
        if (tokenWidget->tokens()->tokenShow(i)->isChecked())
        {
            TokenStore::i()->removeToken(tokenWidget->tokens()->tokenLabel(i)->text().toUtf8().constData());
        }
    }

    auto status = TokenDatabase::saveTokens();
    if (status != TokenDatabase::Success)
    {
        QMessageBox::critical(this, "Error", QString(TokenDatabase::getErrorMessage(status).c_str()));
    }

    this->updateTokenList();
}

void MainWindow::editSelectedTokens()
{
    if (tokenEditor)
    {
        tokenEditor->show();
        tokenEditor->activateWindow();
    }
    else
    {
        tokenEditor = std::make_shared<TokenEditor>(OTPWidget::Mode::Override);
        tokenEditorHelper = std::make_shared<FramelessContainer>(tokenEditor.get());
        QObject::connect(tokenEditor.get(), &WidgetBase::closed, this, [&]{
            tokenEditor.reset();
            tokenEditorHelper.reset();
        });
        QObject::connect(tokenEditor.get(), &TokenEditor::tokensSaved, this, &MainWindow::updateTokenList);

        std::vector<OTPToken*> tokens;
        for (auto i = 0; i < tokenWidget->tokens()->rowCount(); i++)
        {
            if (tokenWidget->tokens()->tokenShow(i)->isChecked())
            {
                tokens.emplace_back(TokenStore::i()->tokenAt(tokenWidget->tokens()->tokenLabel(i)->text().toUtf8().constData()));
            }
        }
        tokenEditor->linkTokens(tokens);

        tokenEditor->show();
        tokenEditor->activateWindow();
    }
}

void MainWindow::exportAllTokens()
{
    if (tokenExporter)
    {
        tokenExporter->show();
        tokenExporter->activateWindow();
    }
    else
    {
        tokenExporter = std::make_shared<TokenEditor>(OTPWidget::Mode::Export);
        tokenExporterHelper = std::make_shared<FramelessContainer>(tokenExporter.get());
        QObject::connect(tokenExporter.get(), &WidgetBase::closed, this, [&]{
            tokenExporter.reset();
            tokenExporterHelper.reset();
        });

        std::vector<OTPToken*> tokens;
        for (auto i = 0; i < tokenWidget->tokens()->rowCount(); i++)
        {
            tokens.emplace_back(TokenStore::i()->tokenAt(tokenWidget->tokens()->tokenLabel(i)->text().toUtf8().constData()));
        }
        tokenExporter->linkTokens(tokens);

        tokenExporter->show();
        tokenExporter->activateWindow();
    }
}

void MainWindow::updateCurrentToken()
{
    auto timer = qobject_cast<QTimer*>(sender());
    auto data = static_cast<TokenUserData*>(timer->userData(0));
    // fix period after initial timer creation
    timer->setInterval(static_cast<int>(1000 * data->token->period()));
    // update generated token
    tokenWidget->tokens()->tokenSecret(data->row)->setText(
        data->token->generateToken().c_str()
    );
}

void MainWindow::updateTokenValidities()
{
    for (auto&& timer : timers)
    {
        auto data = static_cast<TokenUserData*>(timer->userData(0));
        qobject_cast<QProgressBar*>(data->validity)->setValue(timer->remainingTime() / 1000);
    }
}

void MainWindow::toggleTokenVisibility(int row, bool visible)
{
    if (!tokenWidget->tokens()->isColumnHidden(3))
    {
        tokenWidget->tokens()->tokenSecretWidget(row)->setVisible(visible);
        tokenWidget->tokens()->tokenSecret(row)->setVisible(visible);
        tokenWidget->tokens()->tokenSecretTimeout(row)->setVisible(visible);
    }
}

void MainWindow::copyTokenToClipboard(int row)
{
    const auto token = tokenWidget->tokens()->tokenSecret(row)->text();
    clipboard->setText(token);
}

void MainWindow::copyTokenToClipboard(const QString &label)
{
    const auto token = TokenStore::i()->tokenAt(label.toUtf8().constData());
    clipboard->setText(QString::fromUtf8(token->generateToken().c_str()));
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
    // Set focus to search bar
    search->setFocus();

    // Set system tray icon visible text
    if (trayShowHide)
    {
        trayShowHide->setText(trayHideText);
    }

    event->accept();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (tokenEditor)
    {
        tokenEditor->close();
    }
    if (tokenExporter)
    {
        tokenExporter->close();
    }

    // Save UI state
    gcfg::settings()->setValue(gcfg::keyGeometryMainWindow(), saveGeometry());
    QList<int> columns;
    for (auto i = 0; i < tokenWidget->tokens()->columnCount() - 1; i++)
    {
        columns.append(tokenWidget->tokens()->columnWidth(i));
    }
    auto tokenColumnWidth = tokenWidget->tokens()->columnWidth(3);
    if (tokenColumnWidth <= 0)
    {
        tokenColumnWidth = 120;
    }
    columns.append(tokenColumnWidth);
    gcfg::settings()->setValue(gcfg::keyTokenWidgetColumns(), QVariant::fromValue(columns));
    gcfg::settings()->setValue(gcfg::keyTokensVisible(), tokenWidget->tokenVisibilityAction()->isChecked());
    gcfg::settings()->setValue(gcfg::keyTokenIconsVisible(), tokenWidget->tokenIconVisibilityAction()->isChecked());
    gcfg::settings()->sync();

    qApp->quit();
    event->accept();
}

void MainWindow::filterTokens(const QString &searchTerms)
{
    // allow to search by regular expression
    // example: "term 1|term 2" will match both "term 1" and "term 2"
    // OTP types and labels are queried
    const auto&& terms = QRegularExpression(searchTerms.trimmed(), QRegularExpression::CaseInsensitiveOption);

    if (searchTerms.simplified().isEmpty() || !terms.isValid())
    {
        for (auto i = 0; i < tokenWidget->tokens()->rowCount(); i++)
        {
            tokenWidget->tokens()->setRowHidden(i, false);
        }
        return;
    }

    for (auto i = 0; i < tokenWidget->tokens()->rowCount(); i++)
    {
        bool match = false;
        const auto type = tokenWidget->tokens()->tokenType(i)->text().simplified();
        const auto label = tokenWidget->tokens()->tokenLabel(i)->text().simplified();
        if (terms.match(type).hasMatch() ||
            terms.match(label).hasMatch())
        {
            match = true;
        }
        tokenWidget->tokens()->setRowHidden(i, !match);
    }
}
