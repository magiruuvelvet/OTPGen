#ifndef DIALOGBASE_HPP
#define DIALOGBASE_HPP

#include <QDialog>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QBoxLayout>

#include "TitleBar.hpp"

#include <memory>

class DialogBase : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBase(QWidget *parent = nullptr);
    ~DialogBase();

signals:
    void resized();
    void closed();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

    std::shared_ptr<TitleBar> titleBar;

    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;
};

#endif // DIALOGBASE_HPP
