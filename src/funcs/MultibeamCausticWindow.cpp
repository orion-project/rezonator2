#include "MultibeamCausticWindow.h"

#include "../core/Protocol.h"
#include "../math/FunctionGraph.h"
#include "../widgets/PlotHelpers.h"

#include "qcpl_plot.h"
#include "qcpl_io_json.h"

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
    if (_selfSentEvent) return;

    auto graph = _graphs->getBy(qintptr(p));
    if (graph)
    {
        auto pen = graph->pen();
        pen.setColor(p->color());
        graph->setPen(pen);
        _plot->replot();

        if (_graphPens.contains(p))
            _graphPens[p] = graph->pen();
    }
}

void MultibeamCausticWindow::pumpDeleting(Schema*, PumpParams* p)
{
    _graphPens.remove(p);
    if (_lastSelectedPump == p)
        _lastSelectedPump = nullptr;
}

void MultibeamCausticWindow::pumpDeleted(Schema*, PumpParams*)
{
    if (!_lastSelectedPump)
        _lastSelectedPump = schema()->activePump();
    update();
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
        auto graph = _graphs->addMultiGraph(qintptr(pump), pump->label(), workPlane(), funcs);
        graph->setPen(_graphPens.contains(pump) ? _graphPens[pump] : QPen(pump->color()));
    }
    if (errorCount == schema()->pumps()->size())
    {
        showStatusError(tr("No points were calculated, see Protocol window for details"));
        _graphs->clear();
    }
}

void MultibeamCausticWindow::formatMultiGraph(FunctionGraph *graph)
{
    auto pump = (PumpParams*)(graph->id);

    PlotHelpers::FormatPenDlgProps props;
    props.title = tr("Format Line %1").arg(graph->legendName);
    props.onApply = [this, graph, pump](const QPen& pen){
        _graphPens[pump] = pen;
        graph->setPen(pen);
        _plot->replot();
    };
    props.onReset = [this, graph, pump]{
        _graphPens.remove(pump);
        graph->setPen(QPen(pump->color()));
        _plot->replot();
    };
    if (PlotHelpers::formatPenDlg(graph->pen(), props))
    {
        schema()->markModified("MultibeamCausticWindow::formatMultiGraph");

        // Also need to update pump color, it should always be in sync with pen color
        _selfSentEvent = true;
        pump->setColor(graph->pen().color().name());
        schema()->events().raise(SchemaEvents::PumpCustomized, pump, "MultibeamCausticWindow::formatMultiGraph");
        _selfSentEvent = false;
    }
}

void MultibeamCausticWindow::prepareSpecPoints()
{
    if (auto graphLine = selectedGraph(); graphLine)
        if (auto funcGraph = _graphs->findBy(graphLine); funcGraph)
            _lastSelectedPump = (PumpParams*)(funcGraph->id);
    if (!_lastSelectedPump)
        _lastSelectedPump = schema()->activePump();
    if (_lastSelectedPump) {
        function()->setPump(_lastSelectedPump);
        function()->calculate(PlotFunction::CALC_PREPARE);
    }
}

void MultibeamCausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values)
{
    prepareSpecPoints();
    if (!_lastSelectedPump) return;
    if (!function()->ok()) return;
    auto res = function()->calculateAt(pos.X);
    //_beamShape->setShape(res);
    values << CursorInfoValue(CursorInfoValue::SECTION, _lastSelectedPump->label());
    QString valueName = function()->valueSymbol();
    values << CursorInfoValue(valueName+'t', res.T);
    values << CursorInfoValue(valueName+'s', res.S);
    MulticausticWindow::getCursorInfo(pos, values);
}

QString MultibeamCausticWindow::readWindowSpecific(const QJsonObject& root)
{
    auto pensJson = root["graph_pens"].toObject();
    foreach(const QString& label, pensJson.keys())
        if (auto pump = schema()->findPump(label); pump)
            _graphPens[pump] = QCPL::readPen(pensJson[label].toObject(), QPen(pump->color()));
    return {};
}

QString MultibeamCausticWindow::writeWindowSpecific(QJsonObject& root)
{
    QJsonObject pensJson;
    for (auto it = _graphPens.constBegin(); it != _graphPens.constEnd(); it++)
        pensJson[it.key()->label()] = QCPL::writePen(it.value());
    root["graph_pens"] = pensJson;
    return {};
}
