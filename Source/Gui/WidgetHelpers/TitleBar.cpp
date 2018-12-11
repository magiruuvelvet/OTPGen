#include "TitleBar.hpp"
#include "GuiHelpers.hpp"

TitleBar::TitleBar(int minimumHeight, QWidget *parent)
    : QWidget(parent)
{
    this->setMinimumHeight(minimumHeight);

    hbox = GuiHelpers::make_hbox(0, 0, {0, 6, 0, 0});

    hbox->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

    hboxLeftBtns = std::make_shared<QHBoxLayout>();
    hboxLeftBtns->setMargin(0);
    hboxLeftBtns->setContentsMargins(0, 0, 0, 0);
    hboxLeftBtns->setSpacing(0);
    hbox->addLayout(hboxLeftBtns.get());
    hbox->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

    windowTitle = std::make_shared<QLabel>(qApp->applicationDisplayName());
    windowTitle->setMargin(10);
    hbox->addWidget(windowTitle.get(), 0, Qt::AlignVCenter | Qt::AlignHCenter);
    hbox->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

    hboxRightBtns = std::make_shared<QHBoxLayout>();
    hboxRightBtns->setMargin(0);
    hboxRightBtns->setContentsMargins(0, 0, 0, 0);
    hboxRightBtns->setSpacing(0);
    hbox->addLayout(hboxRightBtns.get());

    hbox->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

    this->setLayout(hbox.get());
}

void TitleBar::setLeftButtons(const QList<std::shared_ptr<QPushButton>> &buttons)
{
    for (auto&& btn : buttons)
    {
        hboxLeftBtns->addWidget(btn.get());
    }
}

void TitleBar::setRightButtons(const QList<std::shared_ptr<QPushButton>> &buttons)
{
    for (auto&& btn : buttons)
    {
        hboxRightBtns->addWidget(btn.get());
    }
}

void TitleBar::setWindowTitle(const QString &title)
{
    QString newTitle;

    if (title.isEmpty())
    {
        newTitle = qApp->applicationDisplayName();
    }
    else
    {
        newTitle = GuiHelpers::make_windowTitle(title);
    }

    windowTitle->setText(newTitle);
    QWidget::setWindowTitle(newTitle);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    // FramelessContainer border
    if (event->y() <= 5 || event->x() <= 5 || event->x() >= this->width() - 5)
    {
        return;
    }

    if (this->parentWidget()->isMaximized())
    {
        return;
    }

    this->parentWidget()->move(event->globalX() - m_nMouseClick_X_Coordinate,
                               event->globalY() - m_nMouseClick_Y_Coordinate);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *)
{
    if (this->parentWidget()->isFullScreen())
    {
        return;
    }

    else if (this->parentWidget()->isMaximized())
    {
        this->parentWidget()->showNormal();
    }
    else
    {
        this->parentWidget()->showMaximized();
    }
}
