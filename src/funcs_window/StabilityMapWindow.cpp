#include <QAction>

#include "StabilityMapWindow.h"
#include "../VariableDialog.h"
#include "../widgets/Plot.h"

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
    return Z::Dlgs::editVariable(parent, schema(), function()->arg(), tr("Variable"));
}

void StabilityMapWindow::autolimitsStability()
{
    _plot->yAxis->setRange(-1, 1); // TODO depending on stability calculation mode
    _plot->replot();
}
