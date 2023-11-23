#include "MultibeamCausticWindow.h"

#include "../core/Protocol.h"
#include "../math/FunctionGraph.h"

#include "qcpl_plot.h"

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
    actnFormatGraphT->setVisible(false);
    actnFormatGraphT->setEnabled(false);
    actnFormatGraphS->setVisible(false);
    actnFormatGraphS->setEnabled(false);

    _selectGraphOptions |= SG_UPDATE_CUSROR;
    _selectGraphOptions |= SG_UPDATE_SPEC_POINTS;

    _plot->setDefaultTextY(tr("Beam Radius {(unit)}"));
    _plot->setFormatterTextY(_plot->defaultTextY());
}

Z::WorkPlane MultibeamCausticWindow::workPlane() const
{
    // for this functions T and S modes are exclusive
    return actnShowT->isChecked() ? Z::T : Z::S;
}

void MultibeamCausticWindow::pumpChanged(Schema*, PumpParams* p)
{
    // If pump is active, there will be recalcRequired, so skip double calculation.
    if (!p->isActive()) update();
}

void MultibeamCausticWindow::pumpCustomized(Schema*, PumpParams* p)
{
    if (_skipRecoloring) return;

    auto graph = _graphs->findBy(p->label(), workPlane());
    if (graph)
    {
        auto pen = graph->pen();
        pen.setColor(p->color());
        graph->setPen(pen);
        _plot->replot();
    }
}

void MultibeamCausticWindow::calculate()
{
    _graphs->clear();

    if (!schema()->isSP())
    {
        showStatusError(tr("This function can only operate on SP schema"));
        return;
    }

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
        _graphs->update(pump->label(), workPlane(), funcs, pump->color());
    }
    if (errorCount == schema()->pumps()->size())
    {
        showStatusError(tr("No points were calculated, see Protocol window for details"));
        _graphs->clear();
    }
}

void MultibeamCausticWindow::afterGraphFormatted(FunctionGraph *funcGraph)
{
    auto pump = schema()->findPump(funcGraph->id);
    if (!pump) return;
    QString color = funcGraph->pen().color().name();
    if (color == pump->color()) return;
    pump->setColor(color);
    _skipRecoloring = true;
    schema()->events().raise(SchemaEvents::PumpCustomized, pump, "MultibeamCausticWindow::afterGraphFormatted");
    _skipRecoloring = false;
}

void MultibeamCausticWindow::prepareSpecPoints()
{
    if (auto graphLine = selectedGraph(); graphLine)
        if (auto funcGraph = _graphs->findBy(graphLine); funcGraph)
            _lastSelectedPump = schema()->findPump(funcGraph->id);
    if (!_lastSelectedPump)
        _lastSelectedPump = schema()->activePump();
    function()->setPump(_lastSelectedPump);
    function()->calculate(PlotFunction::CALC_PREPARE);
}

void MultibeamCausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values)
{
    prepareSpecPoints();
    if (!function()->ok()) return;
    auto res = function()->calculateAt(pos.X);
    //_beamShape->setShape(res);
    values << CursorInfoValue(CursorInfoValue::SECTION, _lastSelectedPump->label());
    QString valueName = function()->valueSymbol();
    values << CursorInfoValue(valueName+'t', res.T);
    values << CursorInfoValue(valueName+'s', res.S);
    MulticausticWindow::getCursorInfo(pos, values);
}
