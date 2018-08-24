#include <Config/AppConfig.hpp>

#ifdef OTPGEN_GUI

#include <QApplication>
#include <QMessageBox>
#include <Gui/MainWindow.hpp>
#include <Gui/PasswordInputDialog.hpp>

#include <qtsingleapplication.h>

#include <QFileInfo>

#include <Core/TokenDatabase.hpp>

#include <qtkeychain/keychain.h>

std::shared_ptr<MainWindow> mainWindow;
std::shared_ptr<FramelessContainer> mainWindowContainer;

// QKeychain already handles raw pointers and deletes them
QKeychain::ReadPasswordJob *receivePassword = nullptr;
QKeychain::WritePasswordJob *storePassword = nullptr;

int askPass(const QString &dialogNotice, const QString &error, std::string &password, const QApplication *app = nullptr)
{
    auto passwordDialog = std::make_shared<PasswordInputDialog>();
    password.clear();
    bool hasPassword = false;

    passwordDialog->setDialogNotice(dialogNotice);
    QObject::connect(passwordDialog.get(), &PasswordInputDialog::passwordEntered, app, [&](const QString &pwd){
        password = pwd.toUtf8().constData();
        hasPassword = true;
    });
    passwordDialog->exec();

    if (password.empty() || !hasPassword)
    {
        password.clear();
        QMessageBox::critical(nullptr, "Password missing", error);
        return 1;
    }

    return 0;
}

int start(QtSingleApplication *a, const std::string &keychainPassword, bool create = false)
{
    std::string password;

    // token database exists, ask for decryption and load tokens
    if (QFileInfo(QString::fromUtf8(cfg::database().c_str())).exists())
    {
        // Release Build
#ifndef OTPGEN_DEBUG
        if (keychainPassword.empty())
        {
            int res = askPass("Please enter the decryption password for your token database.",
                              "You didn't entered a password for decryption. Application will quit now.",
                              password, a);

            if (res != 0)
            {
                return res;
            }
        }
        else
        {
            password = keychainPassword;
        }

        TokenDatabase::setPassword(password);

        const auto status = TokenDatabase::loadTokens();
        if (status != TokenDatabase::Success)
        {
            QMessageBox::critical(nullptr, "Error", QString(TokenDatabase::getErrorMessage(status).c_str()));
            return static_cast<int>(status) + 5;
        }

#else
        // Development Build

        TokenDatabase::setPassword("pwd123");
        std::printf("main: loadTokens -> %i\n", TokenDatabase::loadTokens());

        //std::printf("%i\n", TokenDatabase::changePassword("pwd123"));
#endif
    }

    // token database needs to be created, ask for new password
    else
    {
        int res = askPass("No token database found. Please enter a password to create a new database.",
                          "Password may not be empty. Using this application without encryption "
                          "is not supported!",
                          password, a);

        if (res != 0)
        {
            return res;
        }

        TokenDatabase::setPassword(password);

        // save empty database
        TokenDatabase::saveTokens();
    }

    if (create)
    {
        storePassword->setTextData(QString::fromUtf8(password.c_str()));
        QObject::connect(storePassword, &QKeychain::ReadPasswordJob::finished, a, [&]{
            auto error = storePassword->error();

            if (error != QKeychain::NoError)
            {
                QMessageBox::critical(nullptr, "Keychain Error", receivePassword->errorString());
            }
        });
        storePassword->start();
    }
    else
    {
        // not automatically deleted when not used
        delete storePassword;
    }

    password.clear();

    mainWindow = std::make_shared<MainWindow>();
    mainWindowContainer = std::make_shared<FramelessContainer>(mainWindow.get());

    if (!cfg::startMinimizedToTray())
    {
        mainWindow->show();
        mainWindow->activateWindow();
    }

    QObject::connect(a, &QtSingleApplication::messageReceived, a, [&](const QString &message, QObject *socket){
        mainWindow->show();
        mainWindow->activateWindow();
    });

    return 0;
}

int main(int argc, char **argv)
{
    // QApplication::setDesktopSettingsAware(false);
#ifdef OTPGEN_DEBUG
    QtSingleApplication a(cfg::q(cfg::Name) + "_DEBUG", argc, argv);
#else
    QtSingleApplication a(cfg::q(cfg::Name), argc, argv);
#endif
    a.setOrganizationName(cfg::q(cfg::Developer));
    a.setApplicationName(cfg::q(cfg::Name));
    a.setApplicationDisplayName(cfg::q(cfg::Name));
    a.setApplicationVersion(cfg::q(cfg::Version));
    a.setUserData(0, new AppIcon());

    if (a.isRunning())
    {
        std::printf("%s is already running.\n", cfg::Name.c_str());
        a.sendMessage("activateWindow");
        return 0;
    }

    // initialize application settings
    std::printf("path: %s\n", cfg::path().toUtf8().constData());
    std::printf("settings: %s\n", cfg::settings()->fileName().toUtf8().constData());
    cfg::initDefaultSettings();

    // set token database path
    TokenDatabase::setTokenFile(cfg::database());

#ifdef OTPGEN_DEBUG
    const auto keychain_service_name = a.applicationDisplayName() + "_d";
#else
    const auto keychain_service_name = a.applicationDisplayName();
#endif
    const QString keychain_secret_key = "secret";

    // setup QKeychain
    receivePassword = new QKeychain::ReadPasswordJob(keychain_service_name, &a);
    storePassword = new QKeychain::WritePasswordJob(keychain_service_name, &a);
    receivePassword->setKey(keychain_secret_key);
    storePassword->setKey(keychain_secret_key);

    QObject::connect(receivePassword, &QKeychain::ReadPasswordJob::finished, &a, [&]{
        auto error = receivePassword->error();

        // success, use password from keychain
        if (error == QKeychain::NoError)
        {
            auto res = start(&a, receivePassword->textData().toUtf8().constData());
            if (res != 0)
            {
                qApp->exit(res);
            }
        }

        // no entry found, create a new one
        else if (error == QKeychain::EntryNotFound)
        {
            auto res = start(&a, "", true);
            if (res != 0)
            {
                qApp->exit(res);
            }
        }

        // silently fallback to password input on these errors
        else if (error == QKeychain::AccessDeniedByUser ||
                 error == QKeychain::AccessDenied ||
                 error == QKeychain::NoBackendAvailable ||
                 error == QKeychain::NotImplemented)
        {
            auto res = start(&a, "");
            if (res != 0)
            {
                qApp->exit(res);
            }
        }

        // other error, show error to user and fallback to password input
        else
        {
            QMessageBox::critical(nullptr, "Keychain Error", receivePassword->errorString());

            auto res = start(&a, "");
            if (res != 0)
            {
                qApp->exit(res);
            }
        }
    });
    receivePassword->start();

    return a.exec();
}

#endif

#ifdef OTPGEN_CLI

#include <cstdio>

int main(int argc, char **argv)
{
    std::printf("Hello!\n");
}

#endif
