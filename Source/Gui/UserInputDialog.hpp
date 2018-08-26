#ifndef USERINPUTDIALOG_HPP
#define USERINPUTDIALOG_HPP

#include "WidgetBase.hpp"
#include "GuiHelpers.hpp"

#include <QTextEdit>

class UserInputDialog : public WidgetBase
{
    Q_OBJECT

public:
    enum EchoMode {
        Default,
        Password,
    };

    explicit UserInputDialog(EchoMode mode = Default, QWidget *parent = nullptr);
    ~UserInputDialog();

    void setEchoMode(EchoMode mode);
    void setDialogNotice(const QString &notice);
    const QString text() const;

    // blocking execution, application modal
    // use textEntered() to receive the entered text
    void exec();

signals:
    void textEntered(const QString &password);

private:
    void sendText();

private:
    std::shared_ptr<TitleBar> titleBar;

    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;
    std::shared_ptr<QHBoxLayout> buttonHBox;

    QList<std::shared_ptr<QPushButton>> buttons;
    QList<std::shared_ptr<QPushButton>> windowControls;

    std::shared_ptr<QTextEdit> dialogNotice;
    std::shared_ptr<QLineEdit> textInput;
};

#endif // USERINPUTDIALOG_HPP
