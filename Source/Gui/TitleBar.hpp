#ifndef TITLEBAR_HPP
#define TITLEBAR_HPP

#include <memory>

#include <QWidget>
#include <QBoxLayout>
#include <QMouseEvent>

#include <QPushButton>
#include <QLabel>

#include <QList>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

    void setLeftButtons(const QList<std::shared_ptr<QPushButton>> &buttons);
    void setRightButtons(const QList<std::shared_ptr<QPushButton>> &buttons);

    void setWindowTitle(const QString &windowTitle);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;

private:
    std::shared_ptr<QHBoxLayout> hbox;

    std::shared_ptr<QLabel> windowTitle;

    std::shared_ptr<QHBoxLayout> hboxLeftBtns;
    std::shared_ptr<QHBoxLayout> hboxRightBtns;
};

#endif // TITLEBAR_HPP
