#include "PumpWindow.h"
#include "core/Schema.h"
#include "widgets/ValuesEditorTS.h"
#include "../orion/OptionsGroup.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

namespace Z {
namespace Dlgs {

bool pumpParams(QWidget *parent, Schema *schema)
{
    PumpWindow dialog(parent, schema);
    if (!PumpWindow::savedGeometry.isNull())
        dialog.restoreGeometry(PumpWindow::savedGeometry);
    bool result = dialog.exec() == QDialog::Accepted;
    PumpWindow::savedGeometry = dialog.saveGeometry();
    return result;
}

} // namespace Dlgs
} // namespace Z

QByteArray PumpWindow::savedGeometry;

PumpWindow::PumpWindow(QWidget *parent, Schema *schema) : RezonatorDialog(parent)
{
    setWindowTitle(tr("Input Beam Parameters"));

    _schema = schema;
    _params = schema->pumpParams();

    _drawing = new QLabel;
    _drawing->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _drawing->setContentsMargins(6, 6, 6, 6);

    ///////// beam parameters
    /*_paramEditors = new ValuesEditorTS;
    _paramEditors->addEditor(1, &_params.raw.param1);
    _paramEditors->addEditor(2, &_params.raw.param2);
    _paramEditors->addEditor(3, &_params.raw.param3);
    _paramEditors->addStretch();
    _paramEditors->addWidget(_drawing);
    _paramEditors->addStretch();*/

    auto groupParams = new QGroupBox;
    groupParams->setTitle("Parameters");
    groupParams->setLayout(_paramEditors);

    ///////// input mode
    auto groupInputType = new Ori::Gui::OptionsGroup(tr("Mode"), true);
    groupInputType->addOption(Z::PumpMode_waist, tr("Waist"));
    groupInputType->addOption(Z::PumpMode_front, tr("Front"));
    groupInputType->addOption(Z::PumpMode_complex, tr("Complex"));
    // TODO groupInputType->addOption(Z::PumpMode_icomplex, tr("Inv. Complex"));
    groupInputType->addOption(Z::PumpMode_vector, tr("Ray Vector"));
    groupInputType->addOption(Z::PumpMode_sections, tr("Two Sections"));
    connect(groupInputType, SIGNAL(optionChecked(int)), this, SLOT(inputTypeChanged(int)));
    groupInputType->setOption(int(_params.mode));
    inputTypeChanged(int(_params.mode));

    auto layoutInput = new QHBoxLayout;
    layoutInput->addWidget(groupInputType);
    layoutInput->addWidget(groupParams);
    mainLayout()->addLayout(layoutInput);
}

void PumpWindow::collect()
{
    _paramEditors->collect();
    _schema->setPumpParams(_params);
    accept();
    close();
}

void PumpWindow::inputTypeChanged(int mode)
{
    _params.mode = Z::PumpMode(mode);
    switch (_params.mode)
    {
    case Z::PumpMode_waist:
        _drawing->setPixmap(QPixmap(":/drawing/pump_waist"));
        _paramEditors->setSymbol(1, QString::fromUtf8("ω<sub>0</sub>"));
        _paramEditors->setSymbol(2, QString::fromUtf8("z<sub>ω</sub>"));
        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
        _paramEditors->setLabel(1, tr("Waist radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
        _paramEditors->setLabel(2, tr("Distance to waist, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::PumpMode_front:
        _drawing->setPixmap(QPixmap(":/drawing/pump_front"));
        _paramEditors->setSymbol(1, QString::fromUtf8("ω"));
        _paramEditors->setSymbol(2, QString::fromUtf8("R"));
        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
        _paramEditors->setLabel(1, tr("Beam radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
        _paramEditors->setLabel(2, tr("Wavefront ROC, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::PumpMode_complex:
        _drawing->setPixmap(QPixmap(":/drawing/pump_complex"));
        _paramEditors->setSymbol(1, QString::fromUtf8("Re"));
        _paramEditors->setSymbol(2, QString::fromUtf8("Im"));
        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
        _paramEditors->setLabel(1, tr("Real part, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        _paramEditors->setLabel(2, tr("Image part, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    /* TODO
    case Z::PumpMode_icomplex:
        _drawing->setPixmap(QPixmap(":/drawing/complex"));
        _paramEditors->setSymbol(1, QString::fromUtf8("Re"));
        _paramEditors->setSymbol(2, QString::fromUtf8("Im"));
        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
        _paramEditors->setLabel(1, tr("Real part, %1").arg(_schema->unitNameTr(Z::Units::Dim_InvLinear)));
        _paramEditors->setLabel(2, tr("Image part, %1").arg(_schema->unitNameTr(Z::Units::Dim_InvLinear)));
        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    */
    case Z::PumpMode_vector:
        _drawing->setPixmap(QPixmap(":/drawing/pump_ray_vector"));
        _paramEditors->setSymbol(1, QString::fromUtf8("y"));
        _paramEditors->setSymbol(2, QString::fromUtf8("V"));
        _paramEditors->setSymbol(3, QString::fromUtf8("z<sub>y</sub>"));
        _paramEditors->setLabel(1, tr("Beam radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
        _paramEditors->setLabel(2, tr("Half angle of divergence, %1").arg(_schema->unitNameTr(Z::Units::Dim_Angle)));
        _paramEditors->setLabel(3, tr("Distance to radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        break;
    case Z::PumpMode_sections:
        _drawing->setPixmap(QPixmap(":/drawing/pump_two_section"));
        _paramEditors->setSymbol(1, QString::fromUtf8("y<sub>1</sub>"));
        _paramEditors->setSymbol(2, QString::fromUtf8("y<sub>2</sub>"));
        _paramEditors->setSymbol(3, QString::fromUtf8("z<sub>y</sub>"));
        _paramEditors->setLabel(1, tr("Beam radius 1, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
        _paramEditors->setLabel(2, tr("Beam radius 2, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
        _paramEditors->setLabel(3, tr("Distance between, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        break;
    }
}
