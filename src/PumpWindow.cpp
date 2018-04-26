#include "PumpWindow.h"

#include "helpers/OriLayouts.h"
#include "widgets/ValueEditor.h"
#include "widgets/ValuesEditorTS.h"
#include "widgets/OriOptionsGroup.h"

#include <QLabel>

using namespace Ori::Layouts;

//------------------------------------------------------------------------------
//                             WaistParamsEditor
//------------------------------------------------------------------------------

class WaistParamsEditor : public PumpParamsEditor<Z::Pump::WaistParams>
{
public:
    WaistParamsEditor(Z::Pump::WaistParams *params) : PumpParamsEditor(), _params(params)
    {
        auto e1 = new ValueEditorTS;
        auto e2 = new ValueEditorTS;
        auto e3 = new ValueEditorTS;
        LayoutV({ e1, e2, e3 }).useFor(this);
    }
private:
    Z::Pump::WaistParams *_params;
};

//------------------------------------------------------------------------------
//                                PumpWindow
//------------------------------------------------------------------------------

bool PumpWindow::edit(QWidget *parent, class Schema* schema)
{
    PumpWindow dlg(parent, schema);
    return dlg.exec() == QDialog::Accepted;
}

PumpWindow::PumpWindow(QWidget *parent, Schema *schema) : RezonatorDialog(Options(DontDeleteOnClose), parent)
{
    setWindowTitle(tr("Input Beam Parameters"));
    setObjectName("PumpWindow");

    _schema = schema;
    _params = schema->pump();

    _drawing = new QLabel;
    _drawing->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _drawing->setContentsMargins(6, 6, 6, 6);

    _paramEditor = new QWidget;

    auto paramsLayout = LayoutV({_paramEditor, _drawing}).setMargin(0).setSpacing(0);

    auto groupParams = new QGroupBox;
    groupParams->setTitle("Parameters");
    groupParams->setLayout(paramsLayout.boxLayout());

    // Input mode
    auto groupInputType = new Ori::Widgets::OptionsGroup(tr("Mode"), true);
    groupInputType->addOption(Z::Pump::Params::Waist, tr("Waist"));
    groupInputType->addOption(Z::Pump::Params::Front, tr("Front"));
    groupInputType->addOption(Z::Pump::Params::Complex, tr("Complex"));
    groupInputType->addOption(Z::Pump::Params::InvComplex, tr("Inv. Complex"));
    groupInputType->addOption(Z::Pump::Params::RayVector, tr("Ray Vector"));
    groupInputType->addOption(Z::Pump::Params::TwoSections, tr("Two Sections"));
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
    _paramEditors[_params.mode]->collect();
    _schema->setPump(_params);
    accept();
    close();
}

void PumpWindow::inputTypeChanged(int mode)
{
    auto mode = static_cast<Z::Pump::Params::Mode>(mode);

    if (_paramEditors.contains(_params.mode))
        _paramEditors[_params.mode]->setVisible(false);
    _params.mode = mode;

    QWidget *newParamEditor = nullptr;
    switch (mode)
    {
    case Z::Pump::Params::Waist:
        _drawing->setPixmap(QPixmap(":/drawing/pump_waist"));
        if (!_paramEditors.contains(mode))
            newParamEditor = new WaistParamsEditor(&_params.waist);
//        _paramEditors->setSymbol(1, QString::fromUtf8("ω<sub>0</sub>"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("z<sub>ω</sub>"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
//        _paramEditors->setLabel(1, tr("Waist radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
//        _paramEditors->setLabel(2, tr("Distance to waist, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
//        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::Pump::Params::Front:
        _drawing->setPixmap(QPixmap(":/drawing/pump_front"));
//        _paramEditors->setSymbol(1, QString::fromUtf8("ω"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("R"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
//        _paramEditors->setLabel(1, tr("Beam radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
//        _paramEditors->setLabel(2, tr("Wavefront ROC, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
//        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::Pump::Params::Complex:
        _drawing->setPixmap(QPixmap(":/drawing/pump_complex"));
//        _paramEditors->setSymbol(1, QString::fromUtf8("Re"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("Im"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
//        _paramEditors->setLabel(1, tr("Real part, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
//        _paramEditors->setLabel(2, tr("Image part, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
//        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::Pump::Params::InvComplex:
        _drawing->setPixmap(QPixmap(":/drawing/pump_complex"));
//        _paramEditors->setSymbol(1, QString::fromUtf8("Re"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("Im"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("M²"));
//        _paramEditors->setLabel(1, tr("Real part, %1").arg(_schema->unitNameTr(Z::Units::Dim_InvLinear)));
//        _paramEditors->setLabel(2, tr("Image part, %1").arg(_schema->unitNameTr(Z::Units::Dim_InvLinear)));
//        _paramEditors->setLabel(3, tr("Beam quality (MI)"));
        break;
    case Z::Pump::Params::RayVector:
        _drawing->setPixmap(QPixmap(":/drawing/pump_ray_vector"));
//        _paramEditors->setSymbol(1, QString::fromUtf8("y"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("V"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("z<sub>y</sub>"));
//        _paramEditors->setLabel(1, tr("Beam radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
//        _paramEditors->setLabel(2, tr("Half angle of divergence, %1").arg(_schema->unitNameTr(Z::Units::Dim_Angle)));
//        _paramEditors->setLabel(3, tr("Distance to radius, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        break;
    case Z::Pump::Params::TwoSections:
        _drawing->setPixmap(QPixmap(":/drawing/pump_two_section"));
//        _paramEditors->setSymbol(1, QString::fromUtf8("y<sub>1</sub>"));
//        _paramEditors->setSymbol(2, QString::fromUtf8("y<sub>2</sub>"));
//        _paramEditors->setSymbol(3, QString::fromUtf8("z<sub>y</sub>"));
//        _paramEditors->setLabel(1, tr("Beam radius 1, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
//        _paramEditors->setLabel(2, tr("Beam radius 2, %1").arg(_schema->unitNameTr(Z::Units::Dim_Beamsize)));
//        _paramEditors->setLabel(3, tr("Distance between, %1").arg(_schema->unitNameTr(Z::Units::Dim_Linear)));
        break;
    }

    if (newParamEditor)
    {
        qobject_cast<QBoxLayout*>(_paramEditor->layout())->insertWidget(0, newParamEditor);
        _paramEditors[_params.mode] = newParamEditor;
    }
    else
        _paramEditors[_params.mode]->setVisible(true);
}
