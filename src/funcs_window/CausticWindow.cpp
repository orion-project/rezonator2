#include <QBoxLayout>
#include <QLabel>

#include "CausticWindow.h"
#include "../VariableDialog.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"

//------------------------------------------------------------------------------
//                              CausticParamsPanel
//------------------------------------------------------------------------------

class CausticOptionsPanel : public FuncOptionsPanel
{
public:
    CausticOptionsPanel(CausticWindow *window) : FuncOptionsPanel(), _window(window)
    {
        // TODO: check if these strings are translated
        Ori::Layouts::LayoutV({
            makeSectionHeader(tr("Function mode")),
            makeModeButton(":/toolbar/plot_w", tr("Beam radius"), int(CausticFunction::Mode::BeamRadius)),
            makeModeButton(":/toolbar/plot_r", tr("Wavefront curvature radius"), int(CausticFunction::Mode::FontRadius)),
            makeModeButton(":/toolbar/plot_v", tr("Beam divergence angle"), int(CausticFunction::Mode::HalfAngle)),
            Ori::Layouts::Stretch()
        }).setSpacing(0).setMargin(0).useFor(this);

        showCurrentMode();
    }

    int currentFunctionMode() const override
    {
        return int(_window->function()->mode());
    }

    void functionModeChanged(int mode) override
    {
        _window->function()->setMode(CausticFunction::Mode(mode));
        _window->requestAutolimits();
        _window->update();
    }

private:
    CausticWindow* _window;
};

//------------------------------------------------------------------------------
//                                CausticWindow
//------------------------------------------------------------------------------

CausticWindow::CausticWindow(Schema *schema) : PlotFuncWindowStorable(new CausticFunction(schema))
{
    setTitleAndIcon(function()->name(), ":/toolbar/func_caustic");
}

bool CausticWindow::configureInternal(QWidget* parent)
{
    return Z::Dlgs::editVariable_ElementRange(parent, schema(), function()->arg(), tr("Range"));
}

QWidget* CausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel(this);
}

void CausticWindow::schemaParamsChanged(Schema *schema)
{
    Q_UNUSED(schema)

// TODO:NEXT-VER
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
