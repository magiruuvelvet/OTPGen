#include <Config/AppConfig.hpp>

#ifdef OTPGEN_GUI

#include <QApplication>
#include <QMessageBox>
#include <Gui/MainWindow.hpp>
#include <Gui/PasswordInputDialog.hpp>

#include <Core/TokenDatabase.hpp>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationDisplayName(cfg::q(cfg::Name));
    a.setApplicationVersion(cfg::q(cfg::Version));
    a.setUserData(0, new AppIcon());

    //
    // TODO
    // Password dialog when db exists
    //

//    auto passwordDialog = std::make_shared<PasswordInputDialog>();
//    std::string password;
//    bool hasPassword = false;

//    passwordDialog->setDialogNotice("Please enter the decryption password for your token database.");
//    QObject::connect(passwordDialog.get(), &PasswordInputDialog::passwordEntered, &a, [&](const QString &pwd){
//        password = pwd.toUtf8().constData();
//        hasPassword = true;
//    });
//    passwordDialog->exec();

//    if (password.empty() || !hasPassword)
//    {
//        password.clear();
//        QMessageBox::critical(nullptr, "Password missing", "You didn't entered a password for decryption. Application will quit now.");
//        return 1;
//    }

    // hardcoded test data
    TokenDatabase::setPassword("pwd123");
    TokenDatabase::setTokenFile("out.cereal.enc");
    std::printf("main: loadTokens -> %i\n", TokenDatabase::loadTokens());

    //std::printf("%i\n", TokenDatabase::changePassword("pwd123"));

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
