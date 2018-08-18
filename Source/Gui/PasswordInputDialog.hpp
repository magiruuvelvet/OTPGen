#ifndef PASSWORDINPUTDIALOG_HPP
#define PASSWORDINPUTDIALOG_HPP

#include "WidgetBase.hpp"
#include "GuiHelpers.hpp"

#include <QTextEdit>

class PasswordInputDialog : public WidgetBase
{
    Q_OBJECT

public:
    explicit PasswordInputDialog(QWidget *parent = nullptr);
    ~PasswordInputDialog();

    void setDialogNotice(const QString &notice);
    const QString password() const;

    // blocking execution, application modal
    // use passwordEntered() to receive entered password
    void exec();

signals:
    void passwordEntered(const QString &password);

private:
    void sendPassword();

private:
    std::shared_ptr<TitleBar> titleBar;

    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;
    std::shared_ptr<QHBoxLayout> buttonHBox;

    QList<std::shared_ptr<QPushButton>> buttons;
    QList<std::shared_ptr<QPushButton>> windowControls;

    std::shared_ptr<QTextEdit> dialogNotice;
    std::shared_ptr<QLineEdit> passwordInput;
};

#endif // PASSWORDINPUTDIALOG_HPP
