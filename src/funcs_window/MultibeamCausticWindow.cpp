#include "MultibeamCausticWindow.h"

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
    for (auto pump : *schema()->pumps())
    {
        function()->setPump(pump);
        function()->calculate();
        if (!function()->ok())
        {
            showStatusError(function()->errorText());
            _graphs->clear();
            return;
        }
        _graphs->update(pump->label(), workPlane, funcs);
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
