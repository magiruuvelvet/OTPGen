#ifndef WIDGETBASE_HPP
#define WIDGETBASE_HPP

#include <QWidget>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QPushButton>

class WidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetBase(QWidget *parent = nullptr);
    ~WidgetBase();

    void setMaxRestoreButton(QPushButton *btn) const;
    const QPushButton *maxRestoreButton() const;
    QPushButton *maxRestoreButton();

    void updateWindowControls();

signals:
    void resized();
    void closed();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    mutable QPushButton *_maxRestoreButton = nullptr;
};

#endif // WIDGETBASE_HPP
