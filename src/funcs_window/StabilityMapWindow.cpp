#include "StabilityMapWindow.h"
#include "../VariableDialog.h"
#include "../widgets/Plot.h"
#include "helpers/OriLayouts.h"

#include <QAction>

class StabilityMapOptionsPanel : public FuncOptionsPanel
{
public:
    StabilityMapOptionsPanel(StabilityMapWindow* window) : FuncOptionsPanel(), _window(window)
    {
        // TODO: check if these strings are translated
        Ori::Layouts::LayoutV({
            makeSectionHeader(tr("Stability parameter")),
            makeModeButton(":/toolbar/formula_stab_normal", tr("Normal"), int(StabilityCalcMode::Normal)),
            makeModeButton(":/toolbar/formula_stab_squared", tr("Squared"), int(StabilityCalcMode::Squared)),
            Ori::Layouts::Stretch()
        }).setSpacing(0).setMargin(0).useFor(this);

        for (auto button : _modeButtons)
            button->setIconSize(QSize(96, 48));

        showCurrentMode();
    }

    int currentFunctionMode() const override
    {
        return int(_window->function()->stabilityCalcMode());
    }

    void functionModeChanged(int mode) override
    {
        _window->function()->setStabilityCalcMode(StabilityCalcMode(mode));
        _window->update();
    }

private:
    StabilityMapWindow* _window;
};


StabilityMapWindow::StabilityMapWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMapFunction(schema))
{
    setWindowIcon(QIcon(":/toolbar/func_stab_map"));
    createControl();
}

void StabilityMapWindow::createControl()
{
    actnStabilityAutolimits = new QAction(tr("Y-axis -> Stability Range", "Plot action"), this);
    actnStabilityAutolimits->setIcon(QIcon(":/toolbar/limits_stab"));
    connect(actnStabilityAutolimits, SIGNAL(triggered()), this, SLOT(autolimitsStability()));

    menuLimits->addSeparator();
    menuLimits->addAction(actnStabilityAutolimits);

    toolbar()->addSeparator();
    toolbar()->addAction(actnStabilityAutolimits);
}

bool StabilityMapWindow::configure(QWidget* parent)
{
    return Z::Dlgs::editVariable(parent, schema(), function()->arg(), tr("Variable"), FUNC_SETTINGS_GROUP(function()));
}

void StabilityMapWindow::autolimitsStability()
{
    switch (function()->stabilityCalcMode())
    {
    case StabilityCalcMode::Normal:
        _plot->yAxis->setRange(-1, 1);
        break;

    case StabilityCalcMode::Squared:
        _plot->yAxis->setRange(0, 1);
        break;
    }
    _plot->replot();
}

QWidget* StabilityMapWindow::makeOptionsPanel()
{
    return new StabilityMapOptionsPanel(this);
}
