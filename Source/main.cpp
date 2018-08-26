#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <Config/AppConfig.hpp>

#include <Core/TokenDatabase.hpp>
#include <Core/TokenStore.hpp>

void exec_commandline_operation(const std::vector<std::string> &args)
{
    if (args.size() > 1)
    {
        if (args.at(1) == "--swap")
        {
            if (args.size() != 4)
            {
                std::cerr << "Swap operation requires 2 labels!" << std::endl;
                std::exit(2);
            }

            const auto res = TokenStore::i()->swapTokens(args.at(2), args.at(3));
            if (res)
            {
                std::printf("Swapped \"%s\" with \"%s\".\n", args.at(2).c_str(), args.at(3).c_str());
                TokenDatabase::saveTokens();
                std::exit(0);
            }
            else
            {
                std::fprintf(stderr, "Swapping of \"%s\" and \"%s\" failed.\n", args.at(2).c_str(), args.at(3).c_str());
                std::exit(3);
            }
        }
        else if (args.at(1) == "--move")
        {
            if (args.size() != 4)
            {
                std::cerr << "Swap operation requires 1 label and a new position or a label (below)!" << std::endl;
                std::exit(2);
            }

            unsigned long newPos = 0;
            bool ok = false, res;
            try {
                newPos = std::stoul(args.at(3));
                ok = true;
            } catch (...) {
                ok = false;
            }
            if (!ok)
            {
                res = TokenStore::i()->moveTokenBelow(args.at(2).c_str(), args.at(3).c_str());
            }
            else
            {
                res = TokenStore::i()->moveToken(args.at(2).c_str(), newPos);
            }

            if (res)
            {
                std::cout << "Move operation successful." << std::endl;
                TokenDatabase::saveTokens();
                std::exit(0);
            }
            else
            {
                std::cerr << "Move operation failed." << std::endl;
                std::exit(3);
            }
        }
    }
}

#ifdef OTPGEN_GUI

#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>

#include <Gui/MainWindow.hpp>
#include <Gui/PasswordInputDialog.hpp>

#include <qtsingleapplication.h>
#include <qtkeychain/keychain.h>

std::shared_ptr<MainWindow> mainWindow;
std::shared_ptr<FramelessContainer> mainWindowContainer;

// QKeychain already handles raw pointers and deletes them
QKeychain::ReadPasswordJob *receivePassword = nullptr;
QKeychain::WritePasswordJob *storePassword = nullptr;

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

    // run command line operation if any
    const auto args = a->arguments();
    exec_commandline_operation(qtargs_to_strvec(args));

    mainWindow = std::make_shared<MainWindow>();
    mainWindowContainer = std::make_shared<FramelessContainer>(mainWindow.get());

    if (!cfg::startMinimizedToTray())
    {
        mainWindow->show();
        mainWindow->activateWindow();
    }

    // process messages sent from additional instances
    QObject::connect(a, &QtSingleApplication::messageReceived, a, [&](const QString &message, QObject *socket){
        if (message.isEmpty() || message.compare("activateWindow", Qt::CaseInsensitive) == 0)
        {
            std::printf("Trying to activate window...\n");
            mainWindow->show();
            mainWindow->activateWindow();
        }
        else if (message.compare("reloadTokens", Qt::CaseInsensitive) == 0)
        {
            std::printf("Trying to reload the token database...\n");
            mainWindow->updateTokenList();
        }
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

int main(int argc, char **argv)
{
    std::printf("Hello!\n");
}

#endif
