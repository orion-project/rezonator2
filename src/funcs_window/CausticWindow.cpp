#include "CausticWindow.h"

#include "../io/z_io_utils.h"
#include "../io/z_io_json.h"
#include "../VariableDialog.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"

#include <QBoxLayout>
#include <QLabel>

//------------------------------------------------------------------------------
//                              CausticParamsPanel
//------------------------------------------------------------------------------

class CausticOptionsPanel : public FuncOptionsPanel
{
public:
    CausticOptionsPanel(CausticWindow *window) : FuncOptionsPanel(window), _window(window)
    {
        // TODO: check if these strings are translated
        Ori::Layouts::LayoutV({
            makeSectionHeader(tr("Function mode")),
            makeModeButton(":/toolbar/plot_w", tr("Beam radius"), int(CausticFunction::Mode::BeamRadius)),
            makeModeButton(":/toolbar/plot_r", tr("Wavefront curvature radius"), int(CausticFunction::Mode::FontRadius)),
            //TODO:NEXT-VER makeModeButton(":/toolbar/plot_v", tr("Beam divergence angle"), int(CausticFunction::Mode::HalfAngle)),
            Ori::Layouts::Stretch()
        }).setSpacing(0).setMargin(0).useFor(this);

        showCurrentMode();
    }

    int currentFunctionMode() const override
    {
        return static_cast<int>(_window->function()->mode());
    }

    void functionModeChanged(int mode) override
    {
        _window->function()->setMode(static_cast<CausticFunction::Mode>(mode));
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

bool CausticWindow::configureInternal()
{
    return VariableDialog::ElementRangeDlg(schema(), function()->arg(), tr("Range"), "func_caustic").run();
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

QString CausticWindow::readFunction(const QJsonObject& root)
{
    function()->setMode(Z::IO::Utils::enumFromStr(
        root["mode"].toString(), CausticFunction::BeamRadius));
    auto res = Z::IO::Json::readVariable(root["arg"].toObject(), function()->arg(), schema());
    if (!res.isEmpty())
        return res;
    return QString();
}

QString CausticWindow::writeFunction(QJsonObject& root)
{
    root["mode"] = Z::IO::Utils::enumToStr(function()->mode());
    root["arg"] = Z::IO::Json::writeVariable(function()->arg(), schema());
    return QString();
}
