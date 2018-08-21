#ifndef TOKENTABLEWIDGETROW_HPP
#define TOKENTABLEWIDGETROW_HPP

#include <QWidget>

class TokenTableWidgetRow : public QWidget
{
    Q_OBJECT

public:
    explicit TokenTableWidgetRow(int row, QWidget *parent = nullptr);

private:
    int row;
};

#endif // TOKENTABLEWIDGETROW_HPP
