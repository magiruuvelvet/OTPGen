#include "TokenEditor.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include "PasswordInputDialog.hpp"

#include <Core/TokenDatabase.hpp>

#include <Core/Import/andOTP.hpp>
#include <Core/Import/Authy.hpp>
#include <Core/Import/Steam.hpp>

TokenEditor::TokenEditor(QWidget *parent)
    : WidgetBase(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setPalette(GuiHelpers::make_theme(this->palette()));
    this->setWindowTitle(GuiHelpers::make_windowTitle("Add Tokens"));
    this->setWindowIcon(static_cast<AppIcon*>(qApp->userData(0))->icon);

    // initial window size
    this->resize(957, 290);

    GuiHelpers::centerWindow(this);

    vbox = GuiHelpers::make_vbox();

    importMenu = std::make_shared<QMenu>();
    importActions = {
        GuiHelpers::make_importAction("andOTP", QIcon(":/GuiAssets/logos/andotp.svgz"), this, [&]{
            std::vector<TOTPToken> target;
            auto file = QFileDialog::getOpenFileName(this, "Open andOTP token file", QString(),
                "otp_accounts.json (Plain Text) (otp_accounts.json);;"
                "otp_accounts.json.aes (Encrypted) (otp_accounts.json.aes)");
            if (file.isEmpty() || file.isNull())
            {
                return;
            }

            bool status;
            Import::andOTP::Type type;
            std::string password;
            bool hasPassword = false;

            if (file.endsWith("json", Qt::CaseInsensitive))
            {
                type = Import::andOTP::PlainText;

            }
            else
            {
                type = Import::andOTP::Encrypted;

                auto passwordDialog = std::make_shared<PasswordInputDialog>();
                passwordDialog->setDialogNotice("Please enter the decryption password for the given andOTP token database.");
                QObject::connect(passwordDialog.get(), &PasswordInputDialog::passwordEntered, this, [&](const QString &pwd){
                    password = pwd.toUtf8().constData();
                    hasPassword = true;
                });
                passwordDialog->exec();

                if (password.empty() || !hasPassword)
                {
                    password.clear();
                    QMessageBox::critical(this, "Password missing", "You didn't entered a password for decryption. Import process will be aborted.");
                    return;
                }
            }

            status = Import::andOTP::importTOTP(file.toUtf8().constData(), target, type, password);
            password.clear();
            if (status)
            {
                for(auto&& t : target)
                {
                    addNewToken(&t);
                }
            }
            else
            {
                QMessageBox::critical(this, "Import Error",
                    QString("An error occurred during importing your andOTP tokens! "
                            "If you imported an encrypted file make sure the password is correct.\n\n"
                            "Mode: %1").arg(type == Import::andOTP::PlainText ? "Plain Text Import" : "Encrypted Import"));
            }
        }),
        GuiHelpers::make_importAction("Authy", QIcon(":/GuiAssets/logos/authy.svgz"), this, [&]{
            auto file = QFileDialog::getOpenFileName(this, "Open Authy token file", QString(),
                "com.authy.storage.tokens.authenticator.xml (TOTP Tokens) (com.authy.storage.tokens.authenticator.xml);;"
                "com.authy.storage.tokens.authy.xml (Authy Tokens) (com.authy.storage.tokens.authy.xml);;"
                "com.authy.storage.tokens.authenticator.json (TOTP Tokens) (com.authy.storage.tokens.authenticator.json);;"
                "com.authy.storage.tokens.authy.json (Authy Tokens) (com.authy.storage.tokens.authy.json)");
            if (file.isEmpty() || file.isNull())
            {
                return;
            }

            if (file.endsWith("com.authy.storage.tokens.authenticator.xml", Qt::CaseInsensitive))
            {
                std::vector<TOTPToken> target;
                auto status = Import::Authy::importTOTP(file.toUtf8().constData(), target, Import::Authy::XML);
                if (status)
                {
                    for(auto&& t : target)
                    {
                        addNewToken(&t);
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Import Error", "An error occurred during importing your Authy TOTP tokens!"
                                                                "\n\nFormat: XML");
                }
            }
            else if (file.endsWith("com.authy.storage.tokens.authenticator.json", Qt::CaseInsensitive))
            {
                std::vector<TOTPToken> target;
                auto status = Import::Authy::importTOTP(file.toUtf8().constData(), target, Import::Authy::JSON);
                if (status)
                {
                    for(auto&& t : target)
                    {
                        addNewToken(&t);
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Import Error", "An error occurred during importing your Authy TOTP tokens!"
                                                                "\n\nFormat: JSON");
                }
            }
            else if (file.endsWith("com.authy.storage.tokens.authy.xml", Qt::CaseInsensitive))
            {
                std::vector<AuthyToken> target;
                auto status = Import::Authy::importNative(file.toUtf8().constData(), target, Import::Authy::XML);
                if (status)
                {
                    for(auto&& t : target)
                    {
                        addNewToken(&t);
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Import Error", "An error occurred during importing your Authy native tokens!"
                                                                "\n\nFormat: XML");
                }
            }
            else if (file.endsWith("com.authy.storage.tokens.authy.json", Qt::CaseInsensitive))
            {
                std::vector<AuthyToken> target;
                auto status = Import::Authy::importNative(file.toUtf8().constData(), target, Import::Authy::JSON);
                if (status)
                {
                    for(auto&& t : target)
                    {
                        addNewToken(&t);
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Import Error", "An error occurred during importing your Authy native tokens!"
                                                                "\n\nFormat: JSON");
                }
            }
            else
            {
                QMessageBox::critical(this, "Unknown Authy file", "The given file wasn't recognized as an Authy token file.");
            }
        }),
        GuiHelpers::make_importAction("Steam", QIcon(":/GuiAssets/logos/steam.svgz"), this, [&]{
            SteamToken token("Steam");
            auto file = QFileDialog::getOpenFileName(this, "Open SteamGuard configuration file", QString(),
                "Steamguard* (Steamguard*)");
            if (file.isEmpty() || file.isNull())
            {
                return;
            }

            auto status = Import::Steam::importFromSteamGuard(file.toUtf8().constData(), token);
            if (status)
            {
                addNewToken(&token);
            }
            else
            {
                QMessageBox::critical(this, "Import Error", "An error occurred during importing your Steam token!");
            }
        }),
        GuiHelpers::make_menuSeparator(),
        GuiHelpers::make_importAction("otpauth URI", QIcon(), this, [&]{

        }),
        GuiHelpers::make_importAction("QR Code", GuiHelpers::i()->qr_code_icon, this, [&]{

        }),
    };

    for (auto&& action : importActions)
    {
        importMenu->addAction(action.get());
    }

    buttons = GuiHelpers::make_tokenControls(this,
        true, "New token", [&]{ addNewToken(); },
        false, "", [&]{ }
    );
    buttons.append(GuiHelpers::make_toolbtn(GuiHelpers::i()->import_icon, "Import tokens", this, [&]{
        showImportTokensMenu();
    }));

    windowControls.append(GuiHelpers::make_toolbtn(GuiHelpers::i()->save_icon, "Save tokens", this, [&]{
        saveTokens();
    }));
    windowControls.append(GuiHelpers::make_windowControls(this,
        false, [&]{ },
        false, [&]{ },
        true, [&]{ GuiHelpers::default_closeCallback(this); }
    ));

    titleBar = GuiHelpers::make_titlebar("Add Tokens", buttons, windowControls);
    vbox->addWidget(titleBar.get());

    tokenEditWidget = std::make_shared<OTPWidget>(OTPWidget::Mode::Edit);
    tokenEditWidget->setContentsMargins(3,3,3,3);
    vbox->addWidget(tokenEditWidget.get());

    this->setLayout(vbox.get());
}

TokenEditor::~TokenEditor()
{
    buttons.clear();
    windowControls.clear();
    importActions.clear();
}

void TokenEditor::addNewToken()
{
    auto tokens = tokenEditWidget->tokens();
    const auto row = tokens->rowCount();

    tokens->insertRow(row);

    // row user data:
    //  - Type
    //  - Delete Button

    // setup row data
    tokens->setCellWidget(row, 0, OTPWidget::make_typeCb(row, this,
        [&](int){ updateRow(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row); }));
    tokens->setCellWidget(row, 1, OTPWidget::make_labelInput()); // Label
    tokens->setCellWidget(row, 2, OTPWidget::make_secretInput()); // Secret
    tokens->setCellWidget(row, 3, OTPWidget::make_intInput(3, 15)); // Digits
    tokens->setCellWidget(row, 4, OTPWidget::make_intInput(1, 120)); // Period
    tokens->setCellWidget(row, 5, OTPWidget::make_intInput(1, 0x7FFFFFFF)); // Counter
    tokens->setCellWidget(row, 6, OTPWidget::make_algoCb());
    tokens->setCellWidget(row, 7, OTPWidget::make_delBtn(row, this,
        [&]{ deleteRow(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row); }));

    // update row
    updateRow(row);
}

void TokenEditor::addNewToken(OTPToken *token)
{
    auto tokens = tokenEditWidget->tokens();
    const auto row = tokens->rowCount();

    tokens->insertRow(row);

    // row user data:
    //  - Type
    //  - Delete Button

    // setup row data
    tokens->setCellWidget(row, 0, OTPWidget::make_typeCb(row, this,
        [&](int){ updateRow(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row); }));
    tokens->setCellWidget(row, 1, OTPWidget::make_labelInput()); // Label
    tokens->setCellWidget(row, 2, OTPWidget::make_secretInput()); // Secret
    tokens->setCellWidget(row, 3, OTPWidget::make_intInput(3, 15)); // Digits
    tokens->setCellWidget(row, 4, OTPWidget::make_intInput(1, 120)); // Period
    tokens->setCellWidget(row, 5, OTPWidget::make_intInput(1, 0x7FFFFFFF)); // Counter
    tokens->setCellWidget(row, 6, OTPWidget::make_algoCb());
    tokens->setCellWidget(row, 7, OTPWidget::make_delBtn(row, this,
        [&]{ deleteRow(static_cast<TableWidgetCellUserData*>(this->sender()->userData(0))->row); }));

    // update row
    updateRow(row);

    // set token data
    switch (token->type())
    {
        case OTPToken::TOTP:
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 0))->setCurrentIndex(0);
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 1))->setText(QString::fromUtf8(token->label().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(QString::fromUtf8(token->secret().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText(QString::number(token->digits()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText(QString::number(token->period()));
            setAlgorithmCbIndex(qobject_cast<QComboBox*>(tokens->cellWidget(row, 6)), token->algorithm());
            break;
        case OTPToken::HOTP:
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 0))->setCurrentIndex(1);
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 1))->setText(QString::fromUtf8(token->label().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(QString::fromUtf8(token->secret().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText(QString::number(token->digits()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 5))->setText(QString::number(token->period()));
            setAlgorithmCbIndex(qobject_cast<QComboBox*>(tokens->cellWidget(row, 6)), token->algorithm());
            break;
        case OTPToken::Steam:
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 0))->setCurrentIndex(2);
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 1))->setText(QString::fromUtf8(token->label().c_str()));
            tokens->cellWidget(row, 2)->findChild<QLineEdit*>()->setText(QString::fromUtf8(token->secret().c_str()));
            tokens->cellWidget(row, 2)->findChild<QComboBox*>()->setCurrentIndex(1);
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText(QString::number(token->period()));
            break;
        case OTPToken::Authy:
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 0))->setCurrentIndex(3);
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 1))->setText(QString::fromUtf8(token->label().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(QString::fromUtf8(token->secret().c_str()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText(QString::number(token->digits()));
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText(QString::number(token->period()));
            break;
        case OTPToken::None: break;
    }
}

void TokenEditor::saveTokens()
{
    auto tokens = tokenEditWidget->tokens();

    for (auto i = 0; i < tokens->rowCount(); i++)
    {
        const auto type = static_cast<OTPToken::TokenType>(qobject_cast<QComboBox*>(tokens->cellWidget(i, 0))->currentData().toUInt());
        const auto label = std::string(qobject_cast<QLineEdit*>(tokens->cellWidget(i, 1))->text().toUtf8().constData());

        if (TokenStore::i()->contains(label))
        {
            // TODO: indicate name conflict to user
            continue;
        }

        std::string secret;

        if (type == OTPToken::Steam)
        {
            const auto steamInput = tokens->cellWidget(i, 2);
            auto steamSecret = std::string(steamInput->findChild<QLineEdit*>()->text().toUtf8().constData());
            auto steamSecretBase = steamInput->findChild<QComboBox*>()->currentData().toString();

            if (steamSecretBase == "base64")
            {
                secret = SteamToken::convertBase64Secret(steamSecret);
                if (secret.empty())
                {
                    // avoid empty secret for better error handling
                    secret = steamSecret;
                }
            }
            else
            {
                secret = steamSecret;
            }

            steamSecret.clear();
            steamSecretBase.clear();
        }
        else
        {
            secret = std::string(qobject_cast<QLineEdit*>(tokens->cellWidget(i, 2))->text().toUtf8().constData());
        }

        const auto digits = static_cast<OTPToken::DigitType>(qobject_cast<QLineEdit*>(tokens->cellWidget(i, 3))->text().toUShort());
        const auto period = qobject_cast<QLineEdit*>(tokens->cellWidget(i, 4))->text().toUInt();
        const auto counter = qobject_cast<QLineEdit*>(tokens->cellWidget(i, 5))->text().toUInt();

        OTPToken::ShaAlgorithm algorithm = OTPToken::Invalid;
        if (type == OTPToken::TOTP || type == OTPToken::HOTP)
        {
            algorithm = static_cast<OTPToken::ShaAlgorithm>(qobject_cast<QComboBox*>(tokens->cellWidget(i, 6))->currentData().toUInt());
        }
        else if (type == OTPToken::Steam)
        {
            algorithm = OTPToken::Invalid;
        }
        else if (type == OTPToken::Authy)
        {
            algorithm = static_cast<OTPToken::ShaAlgorithm>(static_cast<TokenAlgorithmUserData*>(qobject_cast<QLabel*>(tokens->cellWidget(i, 6))->userData(0))->algorithm);
        }

        // TODO: don't add invalid tokens; test generation and indicate error to user

        TokenStore::Status tokenStatus;

        switch (type)
        {
            case OTPToken::TOTP:
                tokenStatus = TokenStore::i()->addToken(std::make_shared<TOTPToken>(TOTPToken(
                    label, secret, digits, period, counter, algorithm
                )));
                break;

            case OTPToken::HOTP:
                tokenStatus = TokenStore::i()->addToken(std::make_shared<HOTPToken>(HOTPToken(
                    label, secret, digits, period, counter, algorithm
                )));
                break;

            case OTPToken::Steam:
                tokenStatus = TokenStore::i()->addToken(std::make_shared<SteamToken>(SteamToken(
                    label, secret, digits, period, counter, algorithm
                )));
                break;

            case OTPToken::Authy:
                tokenStatus = TokenStore::i()->addToken(std::make_shared<AuthyToken>(AuthyToken(
                    label, secret, digits, period, counter, algorithm
                )));
                break;

            case OTPToken::None:
                tokenStatus = TokenStore::Nullptr;
                break;
        }

#ifdef OTPGEN_DEBUG
        std::cout << "TokenEditor: label -> " << label << std::endl;
        std::cout << "TokenEditor: secret -> " << secret << std::endl;
        std::cout << "TokenEditor: tokenStatus -> " << tokenStatus << std::endl;
        std::cout << "---" << std::endl;
#endif

        secret.clear();
    }

    auto status = TokenDatabase::saveTokens();
    // TODO: report errors to user

    emit tokensSaved();

    this->close();
}

void TokenEditor::showImportTokensMenu()
{
    importMenu->popup(QCursor::pos());
}

void TokenEditor::updateRow(int row)
{
    auto tokens = tokenEditWidget->tokens();

    // receive token type enum from current row
    const auto typeCb = qobject_cast<QComboBox*>(tokens->cellWidget(row, 0));
    const auto type = static_cast<OTPToken::TokenType>(typeCb->itemData(typeCb->currentIndex()).toUInt());

    // transfer secret to new input widget, convenience feature when importing from
    // external files and adjusting the token type (user preference)
    QString secret;
    const auto child = tokens->cellWidget(row, 2)->findChild<QLineEdit*>();
    if (child)
    {
        secret = child->text();
    }
    else
    {
        secret = qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->text();
    }

    // set token options
    switch (type)
    {
        case OTPToken::TOTP: {
            tokens->setCellWidget(row, 2, OTPWidget::make_secretInput());
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(secret);

            tokens->cellWidget(row, 3)->setEnabled(true);  // Digits
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText("6");
            tokens->cellWidget(row, 4)->setEnabled(true);  // Period
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText("30");
            tokens->cellWidget(row, 5)->setEnabled(false); // Counter
            tokens->setCellWidget(row, 6, OTPWidget::make_algoCb());
            tokens->cellWidget(row, 6)->setEnabled(true);  // Algorithm
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 6))->setCurrentIndex(0);
            }
            break;

        case OTPToken::HOTP: {
            tokens->setCellWidget(row, 2, OTPWidget::make_secretInput());
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(secret);

            tokens->cellWidget(row, 3)->setEnabled(true);  // Digits
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText("6");
            tokens->cellWidget(row, 4)->setEnabled(false); // Period
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText("");
            tokens->cellWidget(row, 5)->setEnabled(true);  // Counter
            tokens->setCellWidget(row, 6, OTPWidget::make_algoCb());
            tokens->cellWidget(row, 6)->setEnabled(true);  // Algorithm
            qobject_cast<QComboBox*>(tokens->cellWidget(row, 6))->setCurrentIndex(0);
            }
            break;

        case OTPToken::Steam: {
            tokens->setCellWidget(row, 2, OTPWidget::make_steamInput());
            tokens->cellWidget(row, 2)->findChild<QLineEdit*>()->setText(secret);

            tokens->cellWidget(row, 3)->setEnabled(false); // Digits
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText("");
            tokens->cellWidget(row, 4)->setEnabled(true);  // Period
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText("30");
            tokens->cellWidget(row, 5)->setEnabled(false); // Counter
            tokens->setCellWidget(row, 6, OTPWidget::make_algoForSteam());
            tokens->cellWidget(row, 6)->setEnabled(false); // Algorithm
            }
            break;

        case OTPToken::Authy: {
            tokens->setCellWidget(row, 2, OTPWidget::make_secretInput());
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 2))->setText(secret);

            tokens->cellWidget(row, 3)->setEnabled(true);  // Digits
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 3))->setText("7");
            tokens->cellWidget(row, 4)->setEnabled(true);  // Period
            qobject_cast<QLineEdit*>(tokens->cellWidget(row, 4))->setText("10");
            tokens->cellWidget(row, 5)->setEnabled(false); // Counter
            tokens->setCellWidget(row, 6, OTPWidget::make_algoForAuthy());
            tokens->cellWidget(row, 6)->setEnabled(true);  // Algorithm
            }
            break;

        case OTPToken::None:
            break;
    }

    secret.clear();
}

void TokenEditor::deleteRow(int row)
{
    auto tokens = tokenEditWidget->tokens();

    // delete row widgets
    for (auto i = 0; i < tokens->columnCount(); i++)
    {
        delete tokens->cellWidget(row, i);
    }

    // delete current row
    tokens->removeRow(row);

    // update row data
    for (auto i = 0; i < tokens->rowCount(); i++)
    {
        static_cast<TableWidgetCellUserData*>(tokens->cellWidget(i, 0)->userData(0))->row = i;
        static_cast<TableWidgetCellUserData*>(tokens->cellWidget(i, 7)->userData(0))->row = i;
    }
}

void TokenEditor::setAlgorithmCbIndex(QComboBox *cb, const OTPToken::ShaAlgorithm &algo)
{
    switch (algo)
    {
        case OTPToken::SHA1:
            cb->setCurrentIndex(0);
            break;
        case OTPToken::SHA256:
            cb->setCurrentIndex(1);
            break;
        case OTPToken::SHA512:
            cb->setCurrentIndex(2);
            break;
        case OTPToken::Invalid:
            break;
    }
}
