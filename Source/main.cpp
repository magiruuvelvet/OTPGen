#include <Config/AppConfig.hpp>

#ifdef OTPGEN_GUI

#include <QApplication>
#include <QMessageBox>
#include <Gui/MainWindow.hpp>
#include <Gui/PasswordInputDialog.hpp>

#include <QFileInfo>

#include <Core/TokenDatabase.hpp>

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

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setOrganizationName(cfg::q(cfg::Developer));
    a.setApplicationDisplayName(cfg::q(cfg::Name));
    a.setApplicationVersion(cfg::q(cfg::Version));
    a.setUserData(0, new AppIcon());

    // initialize application settings
    std::printf("path: %s\n", cfg::path().toUtf8().constData());
    std::printf("settings: %s\n", cfg::settings()->fileName().toUtf8().constData());
    cfg::settings()->sync();

    // set token database path
    TokenDatabase::setTokenFile(cfg::database());

    // token database exists, ask for decryption and load tokens
    if (QFileInfo(QString::fromUtf8(cfg::database().c_str())).exists())
    {
        // Release Build
#ifndef OTPGEN_DEBUG
        std::string password;
        int res = askPass("Please enter the decryption password for your token database.",
                          "You didn't entered a password for decryption. Application will quit now.",
                          password, &a);

        if (res != 0)
        {
            return res;
        }

        TokenDatabase::setPassword(password);
        password.clear();

        const auto status = TokenDatabase::loadTokens();
        // TODO: handle errors

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
        std::string password;
        int res = askPass("No token database found. Please enter a password to create a new database.",
                          "Password may not be empty. Using this application without encryption "
                          "is not supported!",
                          password, &a);

        if (res != 0)
        {
            return res;
        }

        TokenDatabase::setPassword(password);
        password.clear();

        // save empty database
        TokenDatabase::saveTokens();
    }

    MainWindow w;
    FramelessContainer f(&w);
    w.show();

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
