#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

#include <AppConfig.hpp>
#include "GuiConfig.hpp"
#include <CommandLineOperation.hpp>

#include <TokenDatabase.hpp>

#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>

//#include <Windows/MainWindow.hpp>
#include <Windows/UserInputDialog.hpp>

#include <qtsingleapplication.h>

#ifdef QTKEYCHAIN_SUPPORT
#include <qtkeychain/keychain.h>
#endif

Q_DECLARE_METATYPE(QList<int>)

//std::shared_ptr<MainWindow> mainWindow;

#ifdef QTKEYCHAIN_SUPPORT
// QKeychain already handles raw pointers and deletes them
QKeychain::ReadPasswordJob *receivePassword = nullptr;
QKeychain::WritePasswordJob *storePassword = nullptr;
#endif

const std::vector<std::string> qtargs_to_strvec(const QStringList &args)
{
    std::vector<std::string> strvec;
    for (auto&& a : args)
    {
        strvec.emplace_back(a.toUtf8().constData());
    }
    return strvec;
}

int askPass(const QString &dialogNotice, const QString &error, std::string &password, const QApplication *app = nullptr)
{
    auto passwordDialog = std::make_shared<UserInputDialog>(UserInputDialog::Password);
    password.clear();
    bool hasPassword = false;

    passwordDialog->setDialogNotice(dialogNotice);
    QObject::connect(passwordDialog.get(), &UserInputDialog::textEntered, app, [&](const QString &pwd){
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
    if (QFileInfo(QString::fromUtf8(gcfg::database().c_str())).exists())
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
        TokenDatabase::initializeTokens();
    }

#ifdef QTKEYCHAIN_SUPPORT
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
#endif

    password.clear();

    // run command line operation if any
    const auto args = a->arguments();
    exec_commandline_operation(qtargs_to_strvec(args));

    //mainWindow = std::make_shared<MainWindow>();

    if (!gcfg::startMinimizedToTray())
    {
        //mainWindow->show();
        //mainWindow->activateWindow();
    }

    // process messages sent from additional instances
    QObject::connect(a, &QtSingleApplication::messageReceived, a, [&](const QString &message, QObject *socket){
        if (message.isEmpty() || message.compare("activateWindow", Qt::CaseInsensitive) == 0)
        {
            std::printf("Trying to activate window...\n");
            //mainWindow->show();
            //mainWindow->activateWindow();
        }
        else if (message.compare("reloadTokens", Qt::CaseInsensitive) == 0)
        {
            std::printf("Trying to reload the token database...\n");
            if (TokenDatabase::loadTokens() == TokenDatabase::Success)
            {
                std::printf("Updated!\n");
                //mainWindow->updateTokenList();
            }
            else
            {
                std::printf("Failed to update the token database!\n");
            }
        }
    });

    return 0;
}

int main(int argc, char **argv)
{
    // QApplication::setDesktopSettingsAware(false);
#ifdef OTPGEN_DEBUG
    QtSingleApplication a(gcfg::q(cfg::Name) + "_DEBUG", argc, argv);
#else
    QtSingleApplication a(gcfg::q(cfg::Name), argc, argv);
#endif
    a.setOrganizationName(gcfg::q(cfg::Developer));
    a.setApplicationName(gcfg::q(cfg::Name));
    a.setApplicationDisplayName(gcfg::q(cfg::Name));
    a.setApplicationVersion(gcfg::q(cfg::Version));
    a.setUserData(0, new AppIcon());

    qRegisterMetaTypeStreamOperators<QList<int>>("QList<int>");

    // don't allow multiple running instances, but send signals to main instance
    if (a.isRunning())
    {
        if (a.arguments().size() > 1)
        {
            if (a.arguments().at(1) == "--reload")
            {
                std::printf("Reloading token database...\n");
                a.sendMessage("reloadTokens");
            }
        }
        else
        {
            std::printf("%s is already running.\n", cfg::Name.c_str());
            a.sendMessage("activateWindow");
        }

        return 0;
    }

    // initialize application settings
    std::printf("path: %s\n", gcfg::path().toUtf8().constData());
    std::printf("settings: %s\n", gcfg::settings()->fileName().toUtf8().constData());
    gcfg::initDefaultSettings();

    // set token database path
    TokenDatabase::setTokenDatabase(gcfg::database());

#ifdef QTKEYCHAIN_SUPPORT
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
#else // QTKEYCHAIN_SUPPORT
    auto res = start(&a, "");
    if (res != 0)
    {
        return res;
    }
#endif

    return a.exec();
}
