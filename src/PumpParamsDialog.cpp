#include "PumpParamsDialog.h"

#include "core/Pump.h"
#include "core/Utils.h"
#include "widgets/Appearance.h"
#include "widgets/ParamsEditor.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriSelectableTile.h"

#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>

//------------------------------------------------------------------------------
//                             PumpModeDrawing
//------------------------------------------------------------------------------
namespace {

class PumpModeDrawing : public QLabel
{
public:
    PumpModeDrawing()
    {
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setContentsMargins(6, 6, 6, 6);
    }

    void setPumpMode(const Z::PumpMode* pumpMode)
    {
        if (pumpMode)
            setPixmap(QPixmap(pumpMode->drawingPath()));
        else
            clear();
    }
};

} // namespace

//------------------------------------------------------------------------------
//                            PumpParamsDialog
//------------------------------------------------------------------------------

Z::PumpParams* PumpParamsDialog::makeNewPump()
{
    auto drawing = new PumpModeDrawing;

    Ori::Widgets::SelectableTileRadioGroup modesGroup;
    modesGroup.selectionFollowsArrows = true;
    connect(&modesGroup, &Ori::Widgets::SelectableTileRadioGroup::dataSelected, [&](const QVariant& data){
        drawing->setPumpMode(var2ptr<Z::PumpMode*>(data));
    });

    auto modesWidget = new QFrame;
    modesWidget->setFrameShape(QFrame::StyledPanel);
    auto modesLayout = new QHBoxLayout(modesWidget);
    modesLayout->setMargin(0);
    modesLayout->setSpacing(0);
    for (auto mode : Z::Pump::allModes())
    {
        auto modeItem = new Ori::Widgets::SelectableTile;
        modeItem->selectionFollowsFocus = true;
        modeItem->setFrameShape(QFrame::NoFrame);
        modeItem->setPixmap(QIcon(mode->iconPath()).pixmap(32, 32));
        modeItem->setTitle(mode->displayName());
        modeItem->setData(ptr2var(mode));

        modesGroup.addTile(modeItem);
        modesLayout->addWidget(modeItem);
    }

    QWidget content;
    Ori::Layouts::LayoutV({modesWidget, drawing})
            .setMargin(0).setSpacing(12).useFor(&content);

    if (Ori::Dlg::Dialog(&content)
            .withIconPath(":/window_icons/pump")
            .withTitle(tr("Select Pump Mode"))
            .withOkSignal(&modesGroup, SIGNAL(doubleClicked(QVariant)))
            .withContentToButtonsSpacingFactor(2)
            .exec())
    {
        auto mode = var2ptr<Z::PumpMode*>(modesGroup.selectedData());
        return mode ? mode->makePump() : nullptr;
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
    auto pumpMode = Z::Pump::findByModeName(params->modeName());
    if (!pumpMode)
        qCritical() << "Unable to find mode for pump parameters";

    auto pumpModeStr = pumpMode ? pumpMode->displayName() : QString();
    setTitleAndIcon(tr("Input Beam Parameters (%1)").arg(pumpModeStr), ":/toolbar/pump_edit");
    setObjectName("PumpDialog");

    QLabel* iconLabel = new QLabel;
    _editorLabel = new QLineEdit;
    _editorTitle = new QLineEdit;
    Z::Gui::setValueFont(_editorLabel);
    Z::Gui::setValueFont(_editorTitle);

    auto layoutLabelTitle = new QFormLayout;
    layoutLabelTitle->addRow(tr("Label:"), _editorLabel);
    layoutLabelTitle->addRow(tr("Title:"), _editorTitle);
    _editorLabel->setText(params->label());
    _editorTitle->setText(params->title());

    auto layoutCommon = new QHBoxLayout;
    layoutCommon->addWidget(iconLabel);
    layoutCommon->addSpacing(12);
    layoutCommon->addLayout(layoutLabelTitle);

    _paramsEditor = new ParamsEditorTS(params->params());

    auto drawing = new PumpModeDrawing;

    if (pumpMode)
    {
        drawing->setPumpMode(pumpMode);
        iconLabel->setPixmap(QIcon(pumpMode->iconPath()).pixmap(48, 48));
        iconLabel->setToolTip(pumpMode->displayName());
    }

    mainLayout()->addLayout(layoutCommon);
    mainLayout()->addSpacing(6);
    mainLayout()->addWidget(_paramsEditor);
    mainLayout()->addWidget(drawing);

    _editorLabel->setFocus();
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
