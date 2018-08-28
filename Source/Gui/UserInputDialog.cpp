#include "UserInputDialog.hpp"

#include <QEventLoop>

#include <Config/AppConfig.hpp>

UserInputDialog::UserInputDialog(EchoMode mode, QWidget *parent)
    : WidgetBase(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setPalette(GuiHelpers::make_theme(this->palette()));
    this->setWindowTitle(qApp->applicationDisplayName());
    this->setWindowIcon(static_cast<AppIcon*>(qApp->userData(0))->icon);

    // initial window size
    this->resize(400, 160);

    GuiHelpers::centerWindow(this);

    vbox = GuiHelpers::make_vbox();
    innerVBox = GuiHelpers::make_vbox(0, 2, QMargins(4,1,4,4));
    buttonHBox = GuiHelpers::make_hbox(0, 4);

    windowControls = GuiHelpers::make_windowControls(this,
        false, [&]{ },
        false, [&]{ },
        true, [&]{ GuiHelpers::default_closeCallback(this); }
    );

    titleBar = GuiHelpers::make_titlebar("", buttons, windowControls);
    vbox->addWidget(titleBar.get());

    dialogNotice = std::make_shared<QTextEdit>();
    dialogNotice->setFrameRect(QRect());
    dialogNotice->setFrameShadow(QFrame::Plain);
    dialogNotice->setFrameShape(QFrame::NoFrame);
    dialogNotice->setAutoFillBackground(true);
    dialogNotice->setReadOnly(true);
    dialogNotice->setMinimumHeight(50);
    if (cfg::useTheming())
    {
        dialogNotice->setStyleSheet(QString("background-color: %1; color: %2").arg(
                                        cfg::titleBarBackground(),
                                        cfg::titleBarForeground()));
    }
    dialogNotice->setWordWrapMode(QTextOption::WordWrap);
    innerVBox->addWidget(dialogNotice.get());

    textInput = std::make_shared<QLineEdit>();
    textInput->setFrame(false);
    textInput->setAutoFillBackground(true);
    textInput->setContentsMargins(3,0,3,0);
    QObject::connect(textInput.get(), &QLineEdit::returnPressed, this, &UserInputDialog::sendText);
    innerVBox->addWidget(textInput.get());

    this->setEchoMode(mode);

    buttons.append(GuiHelpers::make_toolbtn(QIcon(), QString()));
    buttons.last()->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    if (cfg::useTheming())
    {
        buttons.last()->setStyleSheet("background-color: #777");
    }
    buttons.last()->setText("OK");
    QObject::connect(buttons.last().get(), &QPushButton::clicked, this, &UserInputDialog::sendText);

    buttonHBox->addWidget(buttons.last().get());

    innerVBox->addLayout(buttonHBox.get());

    vbox->addLayout(innerVBox.get());
    vbox->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->setLayout(vbox.get());

    textInput->setFocus();
}

UserInputDialog::~UserInputDialog()
{
    buttons.clear();
    windowControls.clear();
}

void UserInputDialog::setEchoMode(EchoMode mode)
{
    switch (mode)
    {
        case Default:
            textInput->setPlaceholderText(QString());
            textInput->setEchoMode(QLineEdit::Normal);
            break;

        case Password:
            textInput->setPlaceholderText("Password");
            textInput->setEchoMode(QLineEdit::Password);
            break;
    }
}

void UserInputDialog::setDialogNotice(const QString &notice)
{
    dialogNotice->setPlainText(notice);
}

const QString UserInputDialog::text() const
{
    return textInput->text();
}

void UserInputDialog::exec()
{
    this->setWindowModality(Qt::ApplicationModal);
    this->show();
    this->activateWindow();

    QEventLoop block;
    QObject::connect(this, &UserInputDialog::closed, &block, &QEventLoop::quit);
    block.exec();

    this->setWindowModality(Qt::NonModal);
}

void UserInputDialog::sendText()
{
    emit textEntered(textInput->text());
    this->close();
}