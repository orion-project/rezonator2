#include "PumpParamsDialog.h"

#include "../app/Appearance.h"
#include "../app/HelpSystem.h"
#include "../core/Pump.h"
#include "../core/Utils.h"
#include "../widgets/ParamsEditor.h"

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

    void setPumpMode(const PumpMode* pumpMode)
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

PumpParams* PumpParamsDialog::makeNewPump()
{
    auto drawing = new PumpModeDrawing;

    Ori::Widgets::SelectableTileRadioGroup modesGroup;
    modesGroup.selectionFollowsArrows = true;
    connect(&modesGroup, &Ori::Widgets::SelectableTileRadioGroup::dataSelected, [&](const QVariant& data){
        drawing->setPumpMode(var2ptr<PumpMode*>(data));
    });

    auto modesWidget = new QFrame;
    modesWidget->setFrameShape(QFrame::StyledPanel);
    auto modesLayout = new QHBoxLayout(modesWidget);
    modesLayout->setContentsMargins(0, 0, 0, 0);
    modesLayout->setSpacing(0);
    for (auto mode : Pumps::allModes())
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

    if (Ori::Dlg::Dialog(&content, false)
            .withIconPath(":/window_icons/pump")
            .withTitle(tr("Select Pump Mode"))
            .withOnHelp([&modesGroup](){
                auto mode = var2ptr<PumpMode*>(modesGroup.selectedData());
                if (mode) Z::HelpSystem::instance()->showTopic("pump_mode.html#" + mode->helpTopic());
                else Z::HelpSystem::instance()->showTopic("pump_mode.html");
            })
            .withOkSignal(&modesGroup, SIGNAL(doubleClicked(QVariant)))
            .withContentToButtonsSpacingFactor(2)
            .exec())
    {
        auto mode = var2ptr<PumpMode*>(modesGroup.selectedData());
        return mode ? mode->makePump() : nullptr;
    }
    return nullptr;
}

bool PumpParamsDialog::editPump(PumpParams *params)
{
    auto dlg = new PumpParamsDialog(params);
    return dlg->exec() == QDialog::Accepted;
}

PumpParamsDialog::PumpParamsDialog(PumpParams *params, QWidget *parent)
    : RezonatorDialog(Options(UseHelpButton), parent), _params(params)
{
    auto pumpMode = Pumps::findByModeName(params->modeName());
    if (!pumpMode)
        qCritical() << "PumpParamsDialog::PumpParamsDialog(): Unable to find mode for pump parameters";

    auto pumpModeStr = pumpMode ? pumpMode->displayName() : QString();
    setTitleAndIcon(tr("Input Beam Parameters (%1)").arg(pumpModeStr), ":/toolbar/pump_edit");
    setObjectName("PumpDialog");

    QLabel* iconLabel = new QLabel;
    _editorLabel = new QLineEdit;
    _editorTitle = new QLineEdit;
    _editorLabel->setFont(Z::Gui::ValueFont().get());
    _editorTitle->setFont(Z::Gui::ValueFont().get());

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

QString PumpParamsDialog::helpTopic() const
{
    auto pumpMode = Pumps::findByModeName(_params->modeName());
    if (pumpMode)
        return "pump_mode.html#" + pumpMode->helpTopic();
    return "input_beam.html";
}
