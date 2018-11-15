#include "GuiHelpers.hpp"

#include "GuiConfig.hpp"

#include <Tools/zlibTool.hpp>
#include <Tools/SvgTool.hpp>

GuiHelpers::GuiHelpers()
{
    _app_icon = QIcon(":/app-icon.svgz");
    _tray_icon = QIcon(":/tray-icon.png");

    _close_icon = loadIcon(":/close.svgz");
    _minimize_icon = loadIcon(":/minimize.svgz");
    _maximize_icon = loadIcon(":/maximize.svgz");
    _restore_icon = loadIcon(":/restore.svgz");

    _add_icon = loadIcon(":/add.svgz");
    _remove_icon = loadIcon(":/remove.svgz");
    _delete_icon = QIcon(":/close.svgz");
    _save_icon = loadIcon(":/save.svgz");
    _import_icon = loadIcon(":/import.svgz");
    _export_icon = loadIcon(":/export.svgz");
    _qr_code_icon = QIcon(":/qr-code.svgz");
    _copy_content_icon = QIcon(":/copy-content.svgz");
    _edit_icon = loadIcon(":/pencil-edit-button.svgz");
    _info_icon = loadIcon(":/info.svgz");
}

const QIcon GuiHelpers::loadIcon(const QString &path)
{
    const auto color = gcfg::iconColor();
    if (color.compare("default", Qt::CaseInsensitive) == 0)
    {
        return QIcon(path);
    }
    else
    {
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        auto buf = file.readAll();
        file.close();

        auto svg = zlibTool::uncompress(buf.constData(), buf.size());
        SvgTool::changeFillColor(svg, color.toUtf8().constData());

        QPixmap pixmap;
        pixmap.loadFromData(reinterpret_cast<const unsigned char*>(svg.data()), static_cast<uint>(svg.size()));
        return QIcon(pixmap);
    }
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

    if (gcfg::useTheming())
    {
        auto palette = btn->palette();
        palette.setColor(QPalette::Active, QPalette::Button, QColor(gcfg::titleBarButtonBackground()));
        palette.setColor(QPalette::Active, QPalette::ButtonText, QColor(gcfg::titleBarButtonForeground()));
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

QList<std::shared_ptr<QPushButton>> GuiHelpers::make_windowControls(const QWidget *receiver,
                                                                    bool minimize, const std::function<void()> &minimizeCallback,
                                                                    bool close, const std::function<void()> &closeCallback)
{
    QList<std::shared_ptr<QPushButton>> windowControls;
    if (minimize)
    {
        windowControls.append(GuiHelpers::make_toolbtn(i()->_minimize_icon, QObject::tr("Minimize Window"), receiver, minimizeCallback));
    }
    if (close)
    {
        windowControls.append(GuiHelpers::make_toolbtn(i()->_close_icon, QObject::tr("Close Window"), receiver, closeCallback));
    }
    return windowControls;
}

QList<std::shared_ptr<QPushButton>> GuiHelpers::make_tokenControls(const QWidget *receiver,
                                                                   bool add, const QString &tooltip1, const std::function<void()> &addCallback,
                                                                   bool remove, const QString &tooltip2, const std::function<void()> &removeCallback)
{
    QList<std::shared_ptr<QPushButton>> tokenControls;
    if (add)
    {
        tokenControls.append(GuiHelpers::make_toolbtn(i()->_add_icon, tooltip1, receiver, addCallback));
    }
    if (remove)
    {
        tokenControls.append(GuiHelpers::make_toolbtn(i()->_remove_icon, tooltip2, receiver, removeCallback));
    }
    return tokenControls;
}

std::shared_ptr<QAction> GuiHelpers::make_menuSeparator()
{
    auto sep = std::make_shared<QAction>();
    sep->setSeparator(true);
    return sep;
}

std::shared_ptr<QAction> GuiHelpers::make_menuAction(const QString &name, const QIcon &icon,
                                                     const QObject *receiver, const std::function<void()> &callback)
{
    auto action = std::make_shared<QAction>();
    action->setText(name);
    action->setIcon(icon);
    QObject::connect(action.get(), &QAction::triggered, receiver, callback);
    return action;
}

const QPalette GuiHelpers::make_theme(const QPalette &base)
{
    if (gcfg::useTheming())
    {
        auto palette = base;
        palette.setColor(QPalette::All, QPalette::Background, QColor(gcfg::titleBarBackground()));
        palette.setColor(QPalette::All, QPalette::Foreground, QColor(gcfg::titleBarForeground()));
        return palette;
    }
    return base;
}

const QPalette GuiHelpers::make_cb_theme(const QPalette &base)
{
    if (gcfg::useTheming())
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
