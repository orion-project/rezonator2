#include "MultibeamCausticWindow.h"

#include "../core/Protocol.h"
#include "../funcs/FunctionGraph.h"

#include <qcpl_plot.h>

#include <QAction>
#include <QDebug>

MultibeamCausticWindow::MultibeamCausticWindow(Schema *schema) : MulticausticWindow(new MultibeamCausticFunction(schema))
{
    _exclusiveModeTS = true;
    _recalcWhenChangeModeTS = true;
    actnShowFlippedTS->setVisible(false);
    actnShowFlippedTS->setEnabled(false);
    actnShowT->setChecked(true);
    actnShowS->setChecked(false);

    _plot->setDefaultTitleY(tr("Beam Radius {(unit)}"));
    _plot->setFormatterTextY(tr("Beam Radius {(unit)}"));
}

void MultibeamCausticWindow::pumpChanged(Schema*, PumpParams* p)
{
    // If pump is active, there will be recalcRequired, so skip double calculation.
    if (!p->isActive()) update();
}

void MultibeamCausticWindow::calculate()
{
    _graphs->clear();

    if (!schema()->isSP())
    {
        showStatusError(tr("This function can only operate on SP schema"));
        return;
    }

    // for this functions T and S modes are exclusive
    auto workPlane = actnShowT->isChecked() ? Z::Plane_T : Z::Plane_S;

    QList<PlotFunction*> funcs;
    for (auto func : function()->funcs())
        if (!func->arg()->element->disabled())
            funcs << func;

    clearStatusInfo();
    int errorCount = 0;
    foreach (auto pump, *schema()->pumps())
    {
        function()->setPump(pump);
        function()->calculate();
        if (!function()->ok())
        {
            errorCount++;
            Z_ERROR(QString("%1: Pump %2: %3").arg(windowTitle(), pump->label(), function()->errorText()));
            continue;
        }
        // TODO: show different pump lines in legend
        // It'll require additional work as a caustic line consist of several ranges
        // which of them is a separate QCP-graph
        _graphs->update(pump->label(), workPlane, funcs, pump->color());
    }
    if (errorCount == schema()->pumps()->size())
    {
        showStatusError(tr("No points were calculated, see Protocol window for details"));
        _graphs->clear();
    }
}
