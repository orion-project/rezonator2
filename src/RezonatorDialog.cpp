#include "RezonatorDialog.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QDialogButtonBox>

QMap<QString, QByteArray> __savedGeometry_RezonatorDialog;

RezonatorDialog::RezonatorDialog(Options options, QWidget* parent) : QDialog(parent? parent: qApp->activeWindow())
{
    if (!(options & DontDeleteOnClose))
        setAttribute(Qt::WA_DeleteOnClose);

    auto layout = new QVBoxLayout(this);

    _mainLayout = new QVBoxLayout;
    _mainLayout->setMargin(0);
    layout->addLayout(_mainLayout);

    if (!(options & OmitButtonsPanel))
    {
        auto buttons = QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
        if (options & UseHelpButton)
            buttons |= QDialogButtonBox::Help;
        auto buttonBox = new QDialogButtonBox(buttons);
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(collect()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        if (options & UseHelpButton)
            connect(buttonBox, &QDialogButtonBox::helpRequested, this, &RezonatorDialog::showHelp);
        layout->addWidget(buttonBox);
    }
}

RezonatorDialog::~RezonatorDialog()
{
    auto name = objectName();
    if (!name.isEmpty())
        __savedGeometry_RezonatorDialog[name] = saveGeometry();
}

void RezonatorDialog::setTitleAndIcon(const QString& title, const char* iconPath)
{
    setWindowTitle(title);
#ifdef Q_OS_MACOS
    // On MacOS the icon of active dialog overrides application icon on the dock.
    Q_UNUSED(iconPath)
#else
    setWindowIcon(QIcon(iconPath));
#endif
}

void RezonatorDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    auto name = objectName();
    if (__savedGeometry_RezonatorDialog.contains(name))
        restoreGeometry(__savedGeometry_RezonatorDialog[name]);
}

void RezonatorDialog::showHelp()
{
    // TODO:NEXT-VER
    qDebug() << "show help" << helpTopic();
}
