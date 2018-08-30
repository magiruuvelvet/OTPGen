#include "DialogBase.hpp"
#include "GuiHelpers.hpp"

DialogBase::DialogBase(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setPalette(GuiHelpers::make_theme(this->palette()));
    this->setWindowTitle(qApp->applicationDisplayName());
    this->setWindowIcon(static_cast<AppIcon*>(qApp->userData(0))->icon);

    vbox = GuiHelpers::make_vbox();
    innerVBox = GuiHelpers::make_vbox(0, 2, QMargins(4,1,4,4));
}

DialogBase::~DialogBase()
{
}

void DialogBase::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

void DialogBase::resizeEvent(QResizeEvent *event)
{
    emit resized();
    event->accept();
}
