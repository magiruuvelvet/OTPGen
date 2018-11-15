#ifndef USERINPUTDIALOG_HPP
#define USERINPUTDIALOG_HPP

#include "GuiHelpers.hpp"

class UserInputDialog : public DialogBase
{
    Q_OBJECT

public:
    enum EchoMode {
        Default,
        Password,
    };

    explicit UserInputDialog(EchoMode mode = Default, QWidget *parent = nullptr);
    ~UserInputDialog() override;

    void setEchoMode(EchoMode mode);
    void setDialogNotice(const QString &notice);
    const QString text() const;

signals:
    void textEntered(const QString &password);

private:
    void sendText();

private:
    std::shared_ptr<QHBoxLayout> buttonHBox;

    QList<std::shared_ptr<QPushButton>> buttons;
    QList<std::shared_ptr<QPushButton>> windowControls;

    std::shared_ptr<QTextEdit> dialogNotice;
    std::shared_ptr<QLineEdit> textInput;
};

#endif // USERINPUTDIALOG_HPP
