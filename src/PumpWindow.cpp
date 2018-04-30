#include "PumpWindow.h"

#include "widgets/ValuesEditorTS.h"
#include "widgets/OriOptionsGroup.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QTimer>

using namespace Ori::Layouts;

namespace {
Z::ValueTS makeValue(const Z::PointTS& point)
{
    return Z::ValueTS(point.T, point.S, Z::Units::none());
}
}

//------------------------------------------------------------------------------
//                             WaistParamsEditor
//------------------------------------------------------------------------------
/*
WaistParamsEditor::WaistParamsEditor(Z::Pump::WaistParams *params) : PumpParamsEditor(), _params(params)
{
    setLayout(new ValuesEditorTS({
        _waist = new ValueEditorTS(tr("Waist radius"), "ω<sub>0</sub>", _params->waist),
        _dist = new ValueEditorTS(tr("Distance to waist"), "z<sub>ω</sub>", _params->distance),
        _MI = new ValueEditorTS(tr("Beam quality"), "M²", makeValue(_params->MI)),
    }));
}
*/
//------------------------------------------------------------------------------
//                             FrontParamsEditor
//------------------------------------------------------------------------------
/*
FrontParamsEditor::FrontParamsEditor(Z::Pump::FrontParams *params) : PumpParamsEditor(), _params(params)
{
    setLayout(new ValuesEditorTS({
        _beam = new ValueEditorTS(tr("Beam radius"), "ω", _params->beamRadius),
        _front = new ValueEditorTS(tr("Wavefront ROC"), "R", _params->frontRadius),
        _MI = new ValueEditorTS(tr("Beam quality"), "M²", makeValue(_params->MI)),
    }));
}
*/
//------------------------------------------------------------------------------
//                                ComplexParamsEditor
//------------------------------------------------------------------------------
/*
ComplexParamsEditor::ComplexParamsEditor(Z::Pump::ComplexParams *params) : PumpParamsEditor(), _params(params)
{
    setLayout(new ValuesEditorTS({
        _re = new ValueEditorTS(tr("Real part"), "Re", makeValue(_params->real)),
        _im = new ValueEditorTS(tr("Imaginary part"), "Im", makeValue(_params->imag)),
        _MI = new ValueEditorTS(tr("Beam quality"), "M²", makeValue(_params->MI)),
    }));
}
*/
//------------------------------------------------------------------------------
//                                PumpWindow
//------------------------------------------------------------------------------
/*
RayVectorParamsEditor::RayVectorParamsEditor(Z::Pump::RayVectorParams *params) : PumpParamsEditor(), _params(params)
{
    setLayout(new ValuesEditorTS({
        _radius = new ValueEditorTS(tr("Beam radius"), "y", _params->radius),
        _angle = new ValueEditorTS(tr("Half angle of divergence"), "V", _params->angle),
        _dist = new ValueEditorTS(tr("Distance to radius"), "z<sub>y</sub>", _params->distance),
    }));
}
*/
//------------------------------------------------------------------------------
//                                PumpWindow
//------------------------------------------------------------------------------
/*
TwoSectionsParamsEditor::TwoSectionsParamsEditor(Z::Pump::TwoSectionsParams *params) : PumpParamsEditor(), _params(params)
{
    setLayout(new ValuesEditorTS({
        _radius1 = new ValueEditorTS(tr("Beam radius 1"), "y<sub>1</sub>", _params->radius1),
        _radius2 = new ValueEditorTS(tr("Beam radius 2"), "y<sub>2</sub>", _params->radius2),
        _dist = new ValueEditorTS(tr("Distance between"), "z<sub>y</sub>", _params->distance),
    }));
}
*/
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
/*
    _schema = schema;
    _params = schema->pump();

    _drawing = new QLabel;
    _drawing->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _drawing->setContentsMargins(6, 6, 6, 6);

    _paramEditorsLayout = LayoutV({_drawing}).setMargin(0).setSpacing(0).boxLayout();

    auto groupParams = new QGroupBox;
    groupParams->setTitle("Parameters");
    groupParams->setLayout(_paramEditorsLayout);

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
    mainLayout()->addLayout(layoutInput);*/
}

void PumpWindow::collect()
{
    //_paramEditors[_params.mode]->collect();
    //_schema->setPump(_params);
    accept();
    close();
}

void PumpWindow::inputTypeChanged(int mode)
{/*
    if (_paramEditors.contains(_params.mode))
        _paramEditors[_params.mode]->setVisible(false);

    _params.mode = static_cast<Z::Pump::Params::Mode>(mode);

    PumpParamsEditor *newParamEditor = nullptr;

    switch (_params.mode)
    {
    case Z::Pump::Params::Waist:
        _drawing->setPixmap(QPixmap(":/drawing/pump_waist"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new WaistParamsEditor(&_params.waist);
        break;
    case Z::Pump::Params::Front:
        _drawing->setPixmap(QPixmap(":/drawing/pump_front"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new FrontParamsEditor(&_params.front);
        break;
    case Z::Pump::Params::Complex:
        _drawing->setPixmap(QPixmap(":/drawing/pump_complex"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new ComplexParamsEditor(&_params.complex);
        break;
    case Z::Pump::Params::InvComplex:
        _drawing->setPixmap(QPixmap(":/drawing/pump_complex"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new ComplexParamsEditor(&_params.icomplex);
        break;
    case Z::Pump::Params::RayVector:
        _drawing->setPixmap(QPixmap(":/drawing/pump_ray_vector"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new RayVectorParamsEditor(&_params.vector);
        break;
    case Z::Pump::Params::TwoSections:
        _drawing->setPixmap(QPixmap(":/drawing/pump_two_section"));
        if (!_paramEditors.contains(_params.mode))
            newParamEditor = new TwoSectionsParamsEditor(&_params.sections);
        break;
    }

    if (newParamEditor)
    {
        _paramEditorsLayout->insertWidget(0, newParamEditor);
        _paramEditors[_params.mode] = newParamEditor;
        QTimer::singleShot(0, [newParamEditor](){
            auto editors = qobject_cast<ValuesEditorTS*>(newParamEditor->layout());
            editors->firstEditor()->setFocus(Z::Plane_T);
            editors->adjustSymbolsWidth();
        });
    }
    else if (_paramEditors.contains(_params.mode))
    {
        _paramEditors[_params.mode]->setVisible(true);
        auto editors = qobject_cast<ValuesEditorTS*>(_paramEditors[_params.mode]->layout());
        editors->firstEditor()->setFocus(Z::Plane_T);
    }
*/}
