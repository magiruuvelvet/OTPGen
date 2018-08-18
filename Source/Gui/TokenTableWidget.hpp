#ifndef TOKENTABLEWIDGET_HPP
#define TOKENTABLEWIDGET_HPP

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPaintEvent>

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

protected:
    void paintEvent(QPaintEvent *event);

private:
    Mode mode;
    bool updating;
    QList<int> invalidRows;
};

#endif // TOKENTABLEWIDGET_HPP
