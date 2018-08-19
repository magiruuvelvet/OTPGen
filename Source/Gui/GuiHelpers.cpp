#include "GuiHelpers.hpp"

#include <Config/AppConfig.hpp>

GuiHelpers::GuiHelpers() :
    app_icon(QIcon(":/GuiAssets/app-icon.svgz")),
    tray_icon(QIcon(":/GuiAssets/tray-icon.png")),

    close_icon(QIcon(":/GuiAssets/close.svgz")),
    minimize_icon(QIcon(":/GuiAssets/minimize.svgz")),
    maximize_icon(QIcon(":/GuiAssets/maximize.svgz")),
    restore_icon(QIcon(":/GuiAssets/restore.svgz")),

    add_icon(QIcon(":/GuiAssets/add.svgz")),
    remove_icon(QIcon(":/GuiAssets/remove.svgz")),
    save_icon(QIcon(":/GuiAssets/save.svgz")),
    import_icon(QIcon(":/GuiAssets/import.svgz")),
    qr_code_icon(QIcon(":/GuiAssets/qr-code.svgz"))
{
}

GuiHelpers *GuiHelpers::i()
{
    static const std::shared_ptr<GuiHelpers> instance(new GuiHelpers());
    return instance.get();
}

const QPoint GuiHelpers::centeredWindowCoords(const QWidget *parent)
{
    const QRect screenGeometry = QApplication::desktop()->screenGeometry(parent);
    auto x = (screenGeometry.width() - parent->width()) / 2;
    auto y = (screenGeometry.height() - parent->height()) / 2;
    return QPoint(x, y);
}

void GuiHelpers::centerWindow(QWidget *parent)
{
    const auto&& pos = centeredWindowCoords(parent);
    parent->move(pos.x(), pos.y());
}

const QString GuiHelpers::make_windowTitle(const QString &title)
{
    return qApp->applicationDisplayName() + QString::fromUtf8(" ─ ") + title;
}

std::shared_ptr<QVBoxLayout> GuiHelpers::make_vbox(int margin, int spacing, const QMargins &contentMargins)
{
    auto vbox = std::make_shared<QVBoxLayout>();
    vbox->setMargin(margin);
    vbox->setContentsMargins(contentMargins);
    vbox->setSpacing(spacing);
    return vbox;
}

std::shared_ptr<QHBoxLayout> GuiHelpers::make_hbox(int margin, int spacing, const QMargins &contentMargins)
{
    auto hbox = std::make_shared<QHBoxLayout>();
    hbox->setMargin(margin);
    hbox->setContentsMargins(contentMargins);
    hbox->setSpacing(spacing);
    return hbox;
}

std::shared_ptr<QPushButton> GuiHelpers::make_toolbtn(const QIcon &icon, const QString &tooltip,
                                                      const QObject *receiver, const std::function<void()> &callback)
{
    auto btn = std::make_shared<QPushButton>();
    btn->setFlat(true);
    btn->setFixedSize(35, 35);
    btn->setIcon(icon);
    btn->setToolTip(tooltip);

    if (cfg::useTheming())
    {
        auto palette = btn->palette();
        palette.setColor(QPalette::Active, QPalette::Button, QColor(cfg::titleBarButtonBackground()));
        palette.setColor(QPalette::Active, QPalette::ButtonText, QColor(cfg::titleBarButtonForeground()));
        // TODO: Inactive, Disabled
        btn->setPalette(palette);
    }

    if (receiver)
    {
        QObject::connect(btn.get(), &QPushButton::clicked, receiver, callback);
    }

    return btn;
}

std::shared_ptr<TitleBar> GuiHelpers::make_titlebar(const QString &windowTitle,
                                                    const QList<std::shared_ptr<QPushButton>> &leftBtns,
                                                    const QList<std::shared_ptr<QPushButton>> &rightBtns)
{
    auto titleBar = std::make_shared<TitleBar>();
    titleBar->setWindowTitle(windowTitle);
    titleBar->setLeftButtons(leftBtns);
    titleBar->setRightButtons(rightBtns);
    return titleBar;
}

QList<std::shared_ptr<QPushButton>> GuiHelpers::make_windowControls(const WidgetBase *receiver,
                                                                    bool minimize, const std::function<void()> &minimizeCallback,
                                                                    bool maximizeRestore, const std::function<void()> &maximizeRestoreCallback,
                                                                    bool close, const std::function<void()> &closeCallback)
{
    QList<std::shared_ptr<QPushButton>> windowControls;
    if (minimize)
    {
        windowControls.append(GuiHelpers::make_toolbtn(i()->minimize_icon, "Minimize Window", receiver, minimizeCallback));
    }
    if (maximizeRestore)
    {
        windowControls.append(GuiHelpers::make_toolbtn(QIcon(), "Maximize/Restore Window", receiver, maximizeRestoreCallback));
        receiver->setMaxRestoreButton(windowControls.last().get());
        QObject::connect(receiver, &WidgetBase::resized, receiver, &WidgetBase::updateWindowControls);
    }
    if (close)
    {
        windowControls.append(GuiHelpers::make_toolbtn(i()->close_icon, "Close Window", receiver, closeCallback));
    }
    return windowControls;
}

QList<std::shared_ptr<QPushButton>> GuiHelpers::make_tokenControls(const WidgetBase *receiver,
                                                                   bool add, const QString &tooltip1, const std::function<void()> &addCallback,
                                                                   bool remove, const QString &tooltip2, const std::function<void()> &removeCallback)
{
    QList<std::shared_ptr<QPushButton>> tokenControls;
    if (add)
    {
        tokenControls.append(GuiHelpers::make_toolbtn(i()->add_icon, tooltip1, receiver, addCallback));
    }
    if (remove)
    {
        tokenControls.append(GuiHelpers::make_toolbtn(i()->remove_icon, tooltip2, receiver, removeCallback));
    }
    return tokenControls;
}

std::shared_ptr<QAction> GuiHelpers::make_menuSeparator()
{
    auto sep = std::make_shared<QAction>();
    sep->setSeparator(true);
    return sep;
}

std::shared_ptr<QAction> GuiHelpers::make_importAction(const QString &name, const QIcon &icon,
                                                       const QObject *receiver, const std::function<void()> &callback)
{
    auto action = std::make_shared<QAction>();
    action->setText(name);
    action->setIcon(icon);
    QObject::connect(action.get(), &QAction::triggered, receiver, callback);
    return action;
}

void GuiHelpers::default_minimizeCallback(WidgetBase *receiver)
{
    receiver->showMinimized();
}

void GuiHelpers::default_maximizeRestoreCallback(WidgetBase *receiver)
{
    if (receiver->isMaximized())
    {
        receiver->showNormal();
        receiver->maxRestoreButton()->setIcon(i()->maximize_icon);
    }
    else
    {
        receiver->showMaximized();
        receiver->maxRestoreButton()->setIcon(i()->restore_icon);
    }
}

void GuiHelpers::default_closeCallback(WidgetBase *receiver)
{
    receiver->close();
}

const QPalette GuiHelpers::make_theme(const QPalette &base)
{
    if (cfg::useTheming())
    {
        auto palette = base;
        palette.setColor(QPalette::All, QPalette::Background, QColor(cfg::titleBarBackground()));
        palette.setColor(QPalette::All, QPalette::Foreground, QColor(cfg::titleBarForeground()));
        return palette;
    }
    return base;
}

const QPalette GuiHelpers::make_cb_theme(const QPalette &base)
{
    if (cfg::useTheming())
    {
        auto palette = base;
        palette.setColor(QPalette::Active, QPalette::Text, QColor("#000000"));
        palette.setColor(QPalette::Active, QPalette::ButtonText, QColor("#000000"));

        palette.setColor(QPalette::Inactive, QPalette::Text, QColor("#999999"));
        palette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor("#999999"));

        palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#CCCCCC"));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#CCCCCC"));
        return palette;
    }
    return base;
}
