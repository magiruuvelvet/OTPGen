#ifndef OTPWIDGET_HPP
#define OTPWIDGET_HPP

#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QProgressBar>

#include "GuiHelpers.hpp"
#include "TokenTableWidget.hpp"

#include <Core/OTPToken.hpp>
#include <Core/TokenStore.hpp>

class OTPWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Mode {
        View,
        Edit,
    };
    enum class Visibility {
        View,
        Edit,
        Both,
    };

    explicit OTPWidget(Mode = Mode::View, QWidget *parent = nullptr);
    ~OTPWidget();

    TokenTableWidget *tokens();

    static QWidget *make_showToggle(int row, const QObject *receiver,
                                    const std::function<void(bool)> &cbCallback,
                                    const std::function<void()> &btnCallback);
    static QComboBox *make_typeCb(int row, const QObject *receiver, const std::function<void(int)> &callback);
    static QComboBox *make_algoCb();
    static QLabel *make_algoForAuthy();
    static QLabel *make_algoForSteam();
    static QLineEdit *make_intInput(int min, int max);
    static QPushButton *make_delBtn(int row, const QObject *receiver, const std::function<void()> &callback);
    static QWidget *make_labelInput(int row, const QObject *receiver, const std::function<void()> &callback,
                                                                      const std::function<void(const QPoint&)> &contextMenu);
    static QLineEdit *make_secretInput();
    static QWidget *make_steamInput();

    static QWidget *make_typeDisplay(const OTPToken *token);
    static QWidget *make_labelDisplay(const std::string &icon, const QString &label);
    static QWidget *make_tokenGenDisplay(const unsigned int &timeout = 0, const OTPToken::TokenType &type = OTPToken::TOTP);

private:
    void showContextMenu(const QPoint &pos);

private:
    std::shared_ptr<QVBoxLayout> vbox;

    std::shared_ptr<QMenu> menu;
    std::shared_ptr<QAction> actionTokenVisibility;

    std::shared_ptr<TokenTableWidget> _tokens;

    // type, label, secret, digits, period, counter, algorithm, token (generated)
    // View: type, label, token (generated)
    // Edit: <all>, except token
    struct Label {
        QString label;
        Visibility visibility;
    };
    QList<Label> labels;
};

class TableWidgetCellUserData : public QObjectUserData
{
public:
    TableWidgetCellUserData(int row)
    { this->row = row; }
    int row = 0;
};

class TokenSecretInputTypeUserData : public QObjectUserData
{
public:
    enum InputType {
        Default,
        Steam,
    };
    TokenSecretInputTypeUserData(const InputType &type)
    { this->type = type; }
    InputType type;
};

class TokenAlgorithmUserData : public QObjectUserData
{
public:
    TokenAlgorithmUserData(const OTPToken::ShaAlgorithm &algorithm)
    { this->algorithm = algorithm; }
    OTPToken::ShaAlgorithm algorithm;
};

class TokenIconUserData : public QObjectUserData
{
public:
    TokenIconUserData(const QString &file)
    { this->file = file; }
    QString file;
};

#endif // OTPWIDGET_HPP
