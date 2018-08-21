#ifndef TOKENEDITOR_HPP
#define TOKENEDITOR_HPP

#include "GuiHelpers.hpp"
#include "OTPWidget.hpp"

class TokenEditor : public WidgetBase
{
    Q_OBJECT

public:
    explicit TokenEditor(OTPWidget::Mode mode = OTPWidget::Mode::Edit, QWidget *parent = nullptr);
    ~TokenEditor();

    void linkTokens(std::vector<OTPToken*> tokens);

signals:
    void tokensSaved();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void addNewToken();
    void addNewToken(OTPToken *token);
    void saveTokens();

    void showImportTokensMenu();

    void updateRow(int row);
    void deleteRow(int row);

    void setAlgorithmCbIndex(QComboBox *cb, const OTPToken::ShaAlgorithm &algo);

    void setTokenIcon(int row);
    void setTokenIcon(int row, const std::string &data);
    void removeTokenIcon(int row);

private:
    OTPWidget::Mode _mode;
    std::vector<OTPToken*> _linkedTokens;

    std::shared_ptr<TitleBar> titleBar;

    std::shared_ptr<QVBoxLayout> vbox;

    std::shared_ptr<QMenu> btnMenu;
    std::shared_ptr<QAction> btnDeleteIcon;

    QList<std::shared_ptr<QPushButton>> buttons;
    QList<std::shared_ptr<QPushButton>> windowControls;

    std::shared_ptr<OTPWidget> tokenEditWidget;

    std::shared_ptr<QMenu> importMenu;
    QList<std::shared_ptr<QAction>> importActions;
};

#endif // TOKENEDITOR_HPP
