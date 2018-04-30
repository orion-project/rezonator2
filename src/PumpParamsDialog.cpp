#include "PumpParamsDialog.h"

#include "core/Pump.h"
#include "widgets/Appearance.h"
#include "widgets/ParamsEditor.h"
#include "helpers/OriDialogs.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>

Z::PumpParams* PumpParamsDialog::makeNewPump()
{
    QListWidget list;
#ifdef Q_OS_WIN
    // Default icon size looks OK on Ubuntu and MacOS but it is too small on Windows
    list.setIconSize(QSize(24, 24));
#endif
    for (Z::PumpMode *mode : Z::Pump::allModes())
    {
        auto it = new QListWidgetItem(QIcon(mode->iconPath()), mode->displayName());
        it->setData(Qt::UserRole, mode->modeName());
        list.addItem(it);
    }
    list.setCurrentRow(0);
    if (Ori::Dlg::Dialog(&list)
            .withIconPath(":/toolbar/schema_pump")
            .withTitle(tr("Select Pump Mode"))
            .exec())
    {
        auto selectedItem = list.currentItem();
        if (!selectedItem) return nullptr;

        auto modeName = selectedItem->data(Qt::UserRole).toString();
        auto mode = Z::Pump::findByModeName(modeName);
        if (!mode) return nullptr;

        Z::PumpParams *pump = mode->makePump();
        if (!editPump(pump))
        {
            delete pump;
            return nullptr;
        }
        return pump;
    }
    return nullptr;
}

bool PumpParamsDialog::editPump(Z::PumpParams *params)
{
    auto dlg = new PumpParamsDialog(params);
    return dlg->exec() == QDialog::Accepted;
}

PumpParamsDialog::PumpParamsDialog(Z::PumpParams *params, QWidget *parent)
    : RezonatorDialog(Options(NoOptions), parent), _params(params)
{
    setTitleAndIcon(tr("Input Beam Parameters"), ":/toolbar/schema_pump");
    setObjectName("PumpDialog");

    _editorLabel = new QLineEdit;
    _editorTitle = new QLineEdit;
    Z::Gui::setValueFont(_editorLabel);
    Z::Gui::setValueFont(_editorTitle);

    auto layoutCommon = new QFormLayout;
    layoutCommon->addRow(tr("Label:"), _editorLabel);
    layoutCommon->addRow(tr("Title:"), _editorTitle);
    _editorLabel->setText(params->label());
    _editorTitle->setText(params->title());

    _paramsEditor = new ParamsEditorTS(params->params());

    auto pumpMode = Z::Pump::findByModeName(params->modeName());
    if (!pumpMode)
        qCritical() << "Unable to find mode for pump parameters";

    auto drawing = new QLabel;
    drawing->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    drawing->setContentsMargins(6, 6, 6, 6);
    if (pumpMode)
        drawing->setPixmap(QPixmap(pumpMode->drawingPath()));

    mainLayout()->addLayout(layoutCommon);
    mainLayout()->addSpacing(6);
    mainLayout()->addWidget(_paramsEditor);
    mainLayout()->addWidget(drawing);
}

void PumpParamsDialog::showEvent(QShowEvent *event)
{
    RezonatorDialog::showEvent(event);

    _paramsEditor->adjustSymbolsWidth();
}

void PumpParamsDialog::collect()
{
    _params->setLabel(_editorLabel->text().trimmed());
    _params->setTitle(_editorTitle->text().trimmed());
    _paramsEditor->collect();
    accept();
    close();
}
