#include "RezonatorDialog.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDialogButtonBox>

QMap<QString, QByteArray> __savedGeometry_RezonatorDialog;

RezonatorDialog::RezonatorDialog(QWidget* parent) : QDialog(parent? parent: qApp->activeWindow())
{
    _mainLayout = new QVBoxLayout;
    _mainLayout->setMargin(0);

    auto buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
        /* TODO provide help topic | QDialogButtonBox::Help */);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(collect()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    auto layout = new QVBoxLayout(this);
    layout->addLayout(_mainLayout);
    layout->addWidget(buttonBox);
}

RezonatorDialog::~RezonatorDialog()
{
    auto name = objectName();
    if (!name.isEmpty())
        __savedGeometry_RezonatorDialog[name] = saveGeometry();
}

void RezonatorDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    auto name = objectName();
    if (__savedGeometry_RezonatorDialog.contains(name))
        restoreGeometry(__savedGeometry_RezonatorDialog[name]);
}
