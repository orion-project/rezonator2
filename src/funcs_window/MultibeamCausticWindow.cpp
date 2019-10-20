#include "MultibeamCausticWindow.h"

#include "../core/Protocol.h"
#include "../funcs/FunctionGraph.h"

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
        funcs << func;

    clearStatusInfo();
    int errorCount = 0;
    for (auto pump : *schema()->pumps())
    {
        function()->setPump(pump);
        function()->calculate();
        if (!function()->ok())
        {
            errorCount++;
            Z_ERROR(QString("%1: Pump %2: %3").arg(windowTitle()).arg(pump->label()).arg(function()->errorText()));
            continue;
        }
        _graphs->update(pump->label(), workPlane, funcs);
    }
    if (errorCount == schema()->pumps()->size())
    {
        showStatusError(tr("No points were calculated, see Protocol window for details"));
        _graphs->clear();
    }
}

QString MultibeamCausticWindow::getDefaultTitle() const
{
    return tr("Beam Radius");
}

QString MultibeamCausticWindow::getDefaultTitleY() const
{
    QString title = tr("Beam radius");
    return QStringLiteral("%1 (%2)").arg(title, getUnitY()->name());
}
