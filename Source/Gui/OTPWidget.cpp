#include "OTPWidget.hpp"

#include <QIntValidator>

OTPWidget::OTPWidget(Mode mode, QWidget *parent)
    : QWidget(parent)
{
    vbox = GuiHelpers::make_vbox();

    _tokens = std::make_shared<TokenTableWidget>(static_cast<TokenTableWidget::Mode>(mode));
    labels = {
        {"Show",        Visibility::View}, // checkbox for "Token"
        {"Type",        Visibility::Both}, // dropdown -or- lineedit (depending on view mode)
        {"Label",       Visibility::Both}, // lineedit
        {"Secret",      Visibility::Edit}, // lineedit
        {"Digits",      Visibility::Edit}, // lineedit (unsigned integer)
        {"Period",      Visibility::Edit}, // lineedit (unsigned integer)
        {"Counter",     Visibility::Edit}, // lineedit (unsigned integer)
        {"Algorithm",   Visibility::Edit}, // dropdown
        {"Token",       Visibility::View}, // lineedit (read only)
        {"Delete",      Visibility::Edit}, // pushbutton (delete)
    };

    _tokens->setFrameShadow(QFrame::Plain);
    _tokens->setFrameShape(QFrame::NoFrame);
    _tokens->setFrameRect(QRect());

    _tokens->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // disable selection highlighting / glow
    _tokens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _tokens->setFocusPolicy(Qt::NoFocus);

    if (mode == Mode::Edit)
    {
        this->setWindowTitle(GuiHelpers::make_windowTitle("Edit Tokens"));

        _tokens->verticalHeader()->hide();
        //_tokens->setSelectionMode(QAbstractItemView::ExtendedSelection);
        //_tokens->setSelectionBehavior(QAbstractItemView::SelectRows);
        _tokens->setSelectionMode(QAbstractItemView::NoSelection);

        _tokens->setColumnCount(8);
        _tokens->setHorizontalHeaderLabels(([&]{
            QStringList headers;
            for (auto&& l : labels)
                if (l.visibility == Visibility::Both ||
                    l.visibility == Visibility::Edit)
                    headers.append(l.label);
            return headers;
        })());

        _tokens->setColumnWidth(0, 100); // Type

        _tokens->setColumnWidth(1, 210); // Label
        _tokens->setColumnWidth(2, 290); // Secret

        _tokens->resizeColumnToContents(3); // Digits
        _tokens->resizeColumnToContents(4); // Period
        _tokens->resizeColumnToContents(5); // Counter

        _tokens->setColumnWidth(6, 100); // Algorithm

        _tokens->resizeColumnToContents(7); // Delete Button
    }
    else
    {
        this->setWindowTitle(GuiHelpers::make_windowTitle("Tokens"));

        _tokens->verticalHeader()->hide();
        _tokens->setSelectionMode(QAbstractItemView::NoSelection);

        _tokens->setColumnCount(4);
        _tokens->setHorizontalHeaderLabels(([&]{
            QStringList headers;
            for (auto&& l : labels)
                if (l.visibility == Visibility::Both ||
                    l.visibility == Visibility::View)
                    headers.append(l.label);
            return headers;
        })());

        _tokens->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(_tokens->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &OTPWidget::showContextMenu);

        menu = std::make_shared<QMenu>();
        actionTokenVisibility = std::make_shared<QAction>();
        actionTokenVisibility->setCheckable(true);
        actionTokenVisibility->setChecked(true);
        actionTokenVisibility->setText("Show Token Column");
        QObject::connect(actionTokenVisibility.get(), &QAction::triggered, this, [&]{
            _tokens->setColumnHidden(3, !actionTokenVisibility->isChecked());
        });
        menu->addAction(actionTokenVisibility.get());

        _tokens->setColumnWidth(0, 68); // Show

        _tokens->setColumnWidth(1, 95); // Type
        _tokens->setColumnWidth(2, 170); // Label
        _tokens->setColumnWidth(3, 120); // Token
    }

    vbox->addWidget(_tokens.get());
    this->setLayout(vbox.get());
}

OTPWidget::~OTPWidget()
{
}

void OTPWidget::showContextMenu(const QPoint &)
{
    menu->popup(QCursor::pos());
}

TokenTableWidget *OTPWidget::tokens()
{
    return _tokens.get();
}

QWidget *OTPWidget::make_showToggle(int row, const QObject *receiver,
                                    const std::function<void(bool)> &cbCallback,
                                    const std::function<void()> &btnCallback)
{
    auto w = new QWidget();
    auto hbox = new QHBoxLayout();
    hbox->setSpacing(0);
    hbox->setMargin(0);
    hbox->setContentsMargins(6,0,3,0);

    auto cb = new QCheckBox();
    cb->setUserData(0, new TableWidgetCellUserData(row));
    cb->setContentsMargins(0,0,0,0);
    QObject::connect(cb, &QCheckBox::toggled, receiver, cbCallback);
    hbox->addWidget(cb);

    auto btn = new QPushButton();
    btn->setUserData(0, new TableWidgetCellUserData(row));
    btn->setFlat(true);
    btn->setFixedSize(23, 23);
    btn->setIcon(GuiHelpers::i()->copy_content_icon);
    btn->setIconSize(QSize(20, 20));
    btn->setToolTip("Copy token to clipboard");
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setContentsMargins(0,0,0,0);
    QObject::connect(btn, &QPushButton::clicked, receiver, btnCallback);
    hbox->addWidget(btn);

    hbox->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    w->setLayout(hbox);
    return w;
}

QComboBox *OTPWidget::make_typeCb(int row, const QObject *receiver, const std::function<void(int)> &callback)
{
    auto cb = new QComboBox();
    cb->setFrame(false);
    cb->setAutoFillBackground(true);
    cb->setPalette(GuiHelpers::make_cb_theme(cb->palette()));
    cb->setStyleSheet("color: black");
    cb->setUserData(0, new TableWidgetCellUserData(row));
    cb->insertItem(0, "  TOTP",  OTPToken::TOTP);
    cb->insertItem(1, "  HOTP",  OTPToken::HOTP);
    cb->insertItem(2, "  Steam", OTPToken::Steam);
    cb->insertItem(3, "  Authy", OTPToken::Authy);

    QObject::connect(cb, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), receiver, callback);
    cb->setCurrentIndex(0);
    return cb;
}

QComboBox *OTPWidget::make_algoCb()
{
    auto cb = new QComboBox();
    cb->setFrame(false);
    cb->setAutoFillBackground(true);
    cb->setPalette(GuiHelpers::make_cb_theme(cb->palette()));
    cb->setStyleSheet("color: black");
    cb->insertItem(0, "  SHA1",   OTPToken::SHA1);
    cb->insertItem(1, "  SHA256", OTPToken::SHA256);
    cb->insertItem(2, "  SHA512", OTPToken::SHA512);
    cb->setCurrentIndex(0);
    return cb;
}

QLabel *OTPWidget::make_algoForAuthy()
{
    auto algo = new QLabel("  SHA1");
    algo->setUserData(0, new TokenAlgorithmUserData(OTPToken::SHA1));
    return algo;
}

QLabel *OTPWidget::make_algoForSteam()
{
    return new QLabel("  SHA1");
}

QLineEdit *OTPWidget::make_intInput(int min, int max)
{
    auto input = new QLineEdit();
    input->setValidator(new QIntValidator(min, max));
    return input;
}

QPushButton *OTPWidget::make_delBtn(int row, const QObject *receiver, const std::function<void()> &callback)
{
    auto btn = new QPushButton();
    btn->setUserData(0, new TableWidgetCellUserData(row));
    btn->setFixedWidth(35);
    btn->setFlat(true);
    btn->setToolTip("Delete token");
    btn->setIcon(GuiHelpers::i()->close_icon);
    QObject::connect(btn, &QPushButton::clicked, receiver, callback);
    return btn;
}

QLineEdit *OTPWidget::make_labelInput()
{
    auto le = new QLineEdit();
    le->setFrame(false);
    le->setAutoFillBackground(true);
    le->setContentsMargins(3,0,3,0);
    return le;
}

QLineEdit *OTPWidget::make_secretInput()
{
    auto le = new QLineEdit();
    le->setFrame(false);
    le->setAutoFillBackground(true);
    le->setContentsMargins(3,0,3,0);
    le->setUserData(0, new TokenSecretInputTypeUserData(TokenSecretInputTypeUserData::Default));
    le->setEchoMode(QLineEdit::Password);
    return le;
}

QWidget *OTPWidget::make_steamInput()
{
    auto w = new QWidget();
    w->setUserData(0, new TokenSecretInputTypeUserData(TokenSecretInputTypeUserData::Steam));
    auto hbox = new QHBoxLayout();
    hbox->setSpacing(0);
    hbox->setMargin(0);
    hbox->setContentsMargins(0,0,0,0);

    auto le = new QLineEdit();
    le->setFrame(false);
    le->setAutoFillBackground(true);
    le->setContentsMargins(3,0,3,0);
    le->setEchoMode(QLineEdit::Password);
    hbox->addWidget(le);

    auto en = new QComboBox();
    en->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    en->setFrame(false);
    en->setAutoFillBackground(true);
    en->setPalette(GuiHelpers::make_cb_theme(en->palette()));
    en->setStyleSheet("color: black");
    en->addItem("  base-64", "base64");
    en->addItem("  base-32", "base32");
    en->setCurrentIndex(0);
    hbox->addWidget(en);

    w->setLayout(hbox);
    return w;
}

QWidget *OTPWidget::make_typeDisplay(const OTPToken *token)
{
    auto w = new QWidget();
    auto hbox = new QHBoxLayout();
    hbox->setSpacing(2);
    hbox->setMargin(0);
    hbox->setContentsMargins(0,0,0,0);

    auto icon = new QLabel();
    icon->setObjectName("icon");
    icon->setFrameShape(QFrame::NoFrame);
    icon->setContentsMargins(7,0,3,0);
    icon->setFixedSize(28, 16);
    icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    switch (token->type())
    {
        case OTPToken::TOTP:  icon->setPixmap(QPixmap(":/GuiAssets/clock.svgz").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)); break;
        case OTPToken::Authy: icon->setPixmap(QPixmap(":/GuiAssets/logos/authy.svgz").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)); break;
        case OTPToken::Steam: icon->setPixmap(QPixmap(":/GuiAssets/logos/steam.svgz").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)); break;
    }
    hbox->addWidget(icon);

    auto label = new QLabel();
    label->setObjectName("name");
    label->setFrameShape(QFrame::NoFrame);
    label->setText(QString::fromUtf8(token->name().c_str()));
    label->setContentsMargins(0,0,7,0);
    hbox->addWidget(label);

    w->setLayout(hbox);
    return w;
}

QWidget *OTPWidget::make_labelDisplay(const std::string &userIcon, const QString &text)
{
    auto w = new QWidget();
    auto hbox = new QHBoxLayout();
    hbox->setSpacing(2);
    hbox->setMargin(0);
    hbox->setContentsMargins(0,0,0,0);

    auto icon = new QLabel();
    icon->setObjectName("icon");
    icon->setFrameShape(QFrame::NoFrame);
    icon->setContentsMargins(7,0,3,0);
    icon->setFixedSize(28, 16);
    icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (!userIcon.empty())
    {
        QPixmap pixmap;
        const auto status = pixmap.loadFromData(reinterpret_cast<const unsigned char*>(userIcon.data()), static_cast<uint>(userIcon.size()));
        if (status)
        {
            icon->setPixmap(pixmap);
        }
    }
    hbox->addWidget(icon);

    auto label = new QLabel();
    label->setObjectName("label");
    label->setFrameShape(QFrame::NoFrame);
    label->setText(text);
    label->setContentsMargins(0,0,7,0);
    hbox->addWidget(label);

    w->setLayout(hbox);
    return w;
}

QWidget *OTPWidget::make_tokenGenDisplay(const unsigned int &to, const OTPToken::TokenType &type)
{
    auto w = new QWidget();
    auto vbox = new QVBoxLayout();
    vbox->setSpacing(0);
    vbox->setMargin(0);
    vbox->setContentsMargins(0,0,0,0);

    auto timeout = new QProgressBar();
    timeout->setAutoFillBackground(true);
    timeout->setOrientation(Qt::Horizontal);
    timeout->setMinimum(0);
    timeout->setMaximum(to == 0 ? 1 : static_cast<int>(to)); // prevent animation and negative inputs
    timeout->setFixedHeight(3);
    timeout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    timeout->setTextVisible(false);
    timeout->setStyleSheet("QProgressBar { background-color: white; border: 0; outline: 0; } QProgressBar::chunk { background-color: lightgray; border: 0; outline: 0; }");
    timeout->setValue(0);
    timeout->setFocusPolicy(Qt::NoFocus);
    if (type == OTPToken::HOTP)
    {
        // HOTP doesn't have a period, set height of progress bar to zero
        timeout->setFixedHeight(0);
    }
    vbox->addWidget(timeout);

    auto le = new QLineEdit();
    le->setReadOnly(true);
    le->setFrame(false);
    le->setAutoFillBackground(true);
    le->setAlignment(Qt::AlignCenter);
    le->setContentsMargins(8,0,8,0);
    auto font = le->font();
    font.setFamily("monospace");
    le->setFont(font);
    le->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vbox->addWidget(le);

    w->setLayout(vbox);
    return w;
}
