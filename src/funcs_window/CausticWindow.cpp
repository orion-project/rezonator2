#include <QBoxLayout>
#include <QLabel>

#include "CausticWindow.h"
#include "../VariableDialog.h"
#include "helpers/OriWidgets.h"

//------------------------------------------------------------------------------
//                              CausticParamsPanel
//------------------------------------------------------------------------------

CausticParamsPanel::CausticParamsPanel(CausticWindow *window): _window(window)
{
    auto modeHeader = new QLabel(tr("<b>Function mode:</b>"));
    modeHeader->setContentsMargins(6, 6, 6, 6);

    Ori::Gui::layoutV(this, 0, 0, {
        modeHeader,
        makeModeButton(":/toolbar/plot_w", tr("Beam radius"), CausticFunction::Mode::Beamsize),
        makeModeButton(":/toolbar/plot_r", tr("Wavefront curvature radius"), CausticFunction::Mode::CurvatureRadius),
        makeModeButton(":/toolbar/plot_v", QString(), CausticFunction::Mode::Angle),
        0
    });

    prepareModes();
    showMode();
}

QWidget* CausticParamsPanel::makeModeButton(const QString& icon, const QString& text, int mode)
{
    auto b = new FunctionModeButton(icon, text, mode);
    connect(b, SIGNAL(clicked()), this, SLOT(modeClicked()));
    _modeButtons.insert(mode, b);
    return b;
}

void CausticParamsPanel::modeClicked()
{
    auto button = qobject_cast<FunctionModeButton*>(sender());
    if (button)
    {
        if (_window->function()->mode() != button->mode())
        {
            _window->function()->setMode(CausticFunction::Mode(button->mode()));
            _window->requestAutolimits();
            _window->update();
        }
        showMode();
    }
}

void CausticParamsPanel::prepareModes()
{
// TODO
//    if (_window->schema()->tripType() == Schema::SP)
//        switch (_window->schema()->pumpParams().mode)
//        {
//        case Z::PumpMode_vector:
//        case Z::PumpMode_sections:
//            prepareModes_ray();
//            break;
//        default:
//            prepareModes_gauss();
//            break;
//        }
//    else prepareModes_gauss();
}

void CausticParamsPanel::prepareModes_ray()
{
    _modeButtons[CausticFunction::Mode::Angle]->setText(tr("Axial angle"));
    _modeButtons[CausticFunction::Mode::CurvatureRadius]->setVisible(false); // there is no sense
}

void CausticParamsPanel::prepareModes_gauss()
{
    _modeButtons[CausticFunction::Mode::Angle]->setText(tr("Angular spread "));
    _modeButtons[CausticFunction::Mode::CurvatureRadius]->setVisible(true);
}

void CausticParamsPanel::showMode()
{
    auto mode = _window->function()->mode();
    for (auto b : _modeButtons)
        b->setChecked(b->mode() == mode);
}

//------------------------------------------------------------------------------
//                                CausticWindow
//------------------------------------------------------------------------------

CausticWindow::CausticWindow(Schema *schema) : PlotFuncWindowStorable(new CausticFunction(schema))
{
    setWindowIcon(QIcon(":/toolbar/func_caustic"));
}

bool CausticWindow::configure(QWidget* parent)
{
    return Z::Dlgs::editVariable_ElementRange(parent, schema(), function()->arg(), tr("Range"));
}

//QWidget* CausticWindow::makeParamsPanel()
//{
//    return nullptr;
//    // TODO check segfault at program closing
//    // return _paramsPanel? _paramsPanel: _paramsPanel = new CausticParamsPanel(this);
//}

void CausticWindow::schemaParamsChanged(Schema *schema)
{
// TODO
//    if (schema->tripType() == Schema::SP)
//        switch (schema->pumpParams().mode)
//        {
//        case Z::PumpMode_vector:
//        case Z::PumpMode_sections:
//            if (function()->mode() == FuncCaustic::Mode::Curvature) // there is no sense
//            {
//                function()->setMode(FuncCaustic::Mode::Beamsize);
//                requestAutolimits();
//            }
//            break;
//        default:
//            break;
//        }
//    if (_paramsPanel)
//    {
//        _paramsPanel->prepareModes();
//        _paramsPanel->showMode();
//    }
}
