#ifndef DIALOGBASE_HPP
#define DIALOGBASE_HPP

#include <QDialog>
#include <QWidget>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QBoxLayout>

#include "GuiHelpers.hpp"
#include "FramelessContainer.hpp"
#include "TitleBar.hpp"

#include <memory>

class QRootWidgetData
{
    friend class QRootWidget;
    friend class QRootDialog;

private:
    // disable creation outside of friend classes
    QRootWidgetData() {}

    static inline void setWindowProperties(QWidget *parent, QRootWidgetData &data)
    {
        parent->setWindowFlag(Qt::FramelessWindowHint, true);
        parent->setPalette(GuiHelpers::make_theme(parent->palette()));
        parent->setWindowTitle(qApp->applicationDisplayName());
        parent->setWindowIcon(GuiHelpers::i()->app_icon());

        data.vbox = GuiHelpers::make_vbox();
        data.innerVBox = GuiHelpers::make_vbox(0, 2, QMargins(4,1,4,4));

        data.framelessContainer = std::make_shared<FramelessContainer>(parent);
    }

public:
    std::shared_ptr<TitleBar> titleBar;
    std::shared_ptr<FramelessContainer> framelessContainer;

    std::shared_ptr<QVBoxLayout> vbox;
    std::shared_ptr<QVBoxLayout> innerVBox;
};

#define QROOTWIDGET_SIGNALS \
    void resized(); \
    void closed();

#define QROOTWIDGET_SLOTS \
    void closeEvent(QCloseEvent *event) \
    { \
        emit closed(); \
        event->accept(); \
    } \
    \
    void resizeEvent(QResizeEvent *event) \
    { \
        emit resized(); \
        event->accept(); \
    }

class QRootWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QRootWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QRootWidgetData::setWindowProperties(this, data);
    }

    ~QRootWidget()
    {
    }

signals:
    QROOTWIDGET_SIGNALS

protected:
    QROOTWIDGET_SLOTS

    QRootWidgetData data;
};

class QRootDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QRootDialog(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        QRootWidgetData::setWindowProperties(this, data);
    }

    ~QRootDialog()
    {
    }

signals:
    QROOTWIDGET_SIGNALS

protected:
    QROOTWIDGET_SLOTS

    QRootWidgetData data;
};

#endif // DIALOGBASE_HPP
