#ifndef TOKENTABLEWIDGET_HPP
#define TOKENTABLEWIDGET_HPP

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPaintEvent>

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QPushButton>

class TokenTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    enum class Mode {
        View,
        Edit,
    };

    explicit TokenTableWidget(Mode mode, QWidget *parent = nullptr);

    void setUpdating(bool);
    void setInvalidRows(const QList<int> &rows);

    inline QWidget *tokenControlWidget(const int row)
    { return cellWidget(row, 0); }

    inline QCheckBox *tokenShow(const int row)
    { return cellWidget(row, 0)->findChild<QCheckBox*>(); }

    inline QPushButton *tokenClipboardButton(const int row)
    { return cellWidget(row, 0)->findChild<QPushButton*>(); }

    inline QLabel *tokenType(const int row)
    { return cellWidget(row, 1)->findChild<QLabel*>("name"); }

    inline QLabel *tokenTypeIcon(const int row)
    { return cellWidget(row, 1)->findChild<QLabel*>("icon"); }

    inline QLabel *tokenLabel(const int row)
    { return cellWidget(row, 2)->findChild<QLabel*>("label"); }

    inline QLabel *tokenIcon(const int row)
    { return cellWidget(row, 2)->findChild<QLabel*>("icon"); }

    inline QWidget *tokenSecretWidget(const int row)
    { return cellWidget(row, 3); }

    inline QLineEdit *tokenSecret(const int row)
    { return cellWidget(row, 3)->findChild<QLineEdit*>(); }

    inline QProgressBar *tokenSecretTimeout(const int row)
    { return cellWidget(row, 3)->findChild<QProgressBar*>(); }


    inline QComboBox *tokenEditType(const int row)
    { return qobject_cast<QComboBox*>(cellWidget(row, 0)); }

    inline QComboBox *tokenAlgorithm(const int row)
    { return qobject_cast<QComboBox*>(cellWidget(row, 6)); }

    inline QLineEdit *tokenDigits(const int row)
    { return qobject_cast<QLineEdit*>(cellWidget(row, 3)); }

    inline QLineEdit *tokenPeriod(const int row)
    { return qobject_cast<QLineEdit*>(cellWidget(row, 4)); }

    inline QLineEdit *tokenCounter(const int row)
    { return qobject_cast<QLineEdit*>(cellWidget(row, 5)); }

    inline QPushButton *tokenDeleteButton(const int row)
    { return qobject_cast<QPushButton*>(cellWidget(row, 7)); }

    inline QLineEdit *tokenEditLabel(const int row)
    { return cellWidget(row, 1)->findChild<QLineEdit*>(); }

    inline QPushButton *tokenEditIcon(const int row)
    { return cellWidget(row, 1)->findChild<QPushButton*>(); }

    inline QLineEdit *tokenEditSecret(const int row)
    {
        auto w = cellWidget(row, 2);
        auto childEdit = w->findChild<QLineEdit*>();
        if (childEdit)
        {
            return childEdit;
        }
        return qobject_cast<QLineEdit*>(w);
    }

    inline QComboBox *tokenEditSecretComboBoxExtra(const int row)
    {
        auto w = cellWidget(row, 2);
        auto childCb = w->findChild<QComboBox*>();
        if (childCb)
        {
            return childCb;
        }
        return nullptr;
    }

protected:
    void paintEvent(QPaintEvent *event);

private:
    Mode mode;
    bool updating;
    QList<int> invalidRows;
};

#endif // TOKENTABLEWIDGET_HPP
