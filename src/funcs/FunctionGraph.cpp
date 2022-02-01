#include "FunctionGraph.h"

#include "PlotFunction.h"

#include "qcpl_plot.h"

//------------------------------------------------------------------------------
//                                 FunctionGraph
//------------------------------------------------------------------------------

FunctionGraph::FunctionGraph(QCPL::Plot* plot, Z::WorkPlane workPlane, std::function<GraphUnits()> getUnits)
    : _plot(plot), _workPlane(workPlane), _getUnits(getUnits)
{
}

void FunctionGraph::clear()
{
    foreach (auto s, _segments)
        _plot->removePlottable(s);
    _segments.clear();
}

void FunctionGraph::update(PlotFunction *function)
{
    if (!_isVisible)
    {
        clear();
        return;
    }
    int segmentCount = function->resultCount(_workPlane);
    for (int i = 0; i < segmentCount; i++)
    {
        auto segment = getOrMakeSegment(i);
        fillSegment(segment, function, i);
    }
    trimToCount(segmentCount);
}

void FunctionGraph::update(const QList<PlotFunction *> &functions)
{
    if (!_isVisible)
    {
        clear();
        return;
    }
    double offset = 0;
    int totalSegmentCount = 0;
    for (auto function : functions)
    {
        int segmentCount = function->resultCount(_workPlane);
        for (int i = 0; i < segmentCount; i++)
        {
            auto segment = getOrMakeSegment(totalSegmentCount + i);
            fillSegment(segment, function, i, offset);
        }
        offset += function->arg()->range.stop.toSi();
        totalSegmentCount += segmentCount;
    }
    trimToCount(totalSegmentCount);
}

QCPL::Graph* FunctionGraph::getOrMakeSegment(int index)
{
    QCPGraph *segment;
    if (index >= _segments.size())
    {
        segment = _plot->addGraph();
        segment->setPen(_linePen);
        segment->setLayer("graphs");
        _segments.append(segment);
    }
    else segment = _segments[index];
    return segment;
}

void FunctionGraph::fillSegment(QCPGraph* segment, PlotFunction* function, int resultIndex, double offsetX)
{
    auto result = function->result(_workPlane, resultIndex);
    int count = result.pointsCount();
    auto xs = result.x();
    auto ys = result.y();
    auto units = _getUnits();
    auto factorY = _isFlipped ? -1 : 1;
    QSharedPointer<QCPGraphDataContainer> data(new QCPGraphDataContainer);
    for (int i = 0; i < count; i++)
    {
        // TODO: possible optimization: extract unit's SI factor before loop
        // and replace the call of virtual method with simple multiplication
        double x = units.X->fromSi(xs.at(i) + offsetX);
        double y = units.Y->fromSi(ys.at(i) * factorY);
        data->add(QCPGraphData(x, y));
    }
    segment->setData(data);
}

void FunctionGraph::trimToCount(int count)
{
    while (_segments.size() > count)
    {
        _plot->removePlottable(_segments.last());
        _segments.removeLast();
    }
}

void FunctionGraph::setColor(Z::WorkPlane workPlane, const QString& color)
{
    QColor clr(color);
    if (!clr.isValid())
        clr = workPlane == Z::Plane_T ? Qt::darkGreen : Qt::red;
    setPen(QPen(clr));
}

bool FunctionGraph::contains(QCPGraph* graph) const
{
    foreach (auto g, segments())
        if (graph == g) return true;
    return false;
}

//------------------------------------------------------------------------------
//                               FunctionGraphSet
//------------------------------------------------------------------------------

FunctionGraphSet::FunctionGraphSet(QCPL::Plot* plot, std::function<GraphUnits()> getUnits): _plot(plot), _getUnits(getUnits)
{
    _graphT = new FunctionGraph(plot, Z::Plane_T, getUnits);
    _graphS = new FunctionGraph(plot, Z::Plane_S, getUnits);
    _graphT->setPen(QPen(Qt::darkGreen));
    _graphS->setPen(QPen(Qt::red));
}

FunctionGraphSet::~FunctionGraphSet()
{
    delete _graphT;
    delete _graphS;

    auto it = _graphs.constBegin();
    while (it != _graphs.constEnd())
    {
        delete it.value();
        it++;
    }
    _graphs.clear();
}

void FunctionGraphSet::clear()
{
    _graphT->clear();
    _graphS->clear();

    auto it = _graphs.constBegin();
    while (it != _graphs.constEnd())
    {
        it.value()->clear();
        it++;
    }
}

void FunctionGraphSet::update(PlotFunction* function)
{
    _graphT->update(function);
    _graphS->update(function);
}

void FunctionGraphSet::update(const QList<PlotFunction*>& functions)
{
    _graphT->update(functions);
    _graphS->update(functions);
}

void FunctionGraphSet::update(const QString& id, Z::WorkPlane workPlane, const QList<PlotFunction*>& functions, const QString &color)
{
    QString key = id + (workPlane == Z::Plane_T ? "_t" : "_s");
    if (!_graphs.contains(key))
    {
        auto graph = new FunctionGraph(_plot, workPlane, _getUnits);
        graph->setColor(workPlane, color);
        _graphs.insert(key, graph);
    }
    else
        _graphs[key]->setColor(workPlane, color);
    _graphs[key]->update(functions);
}

FunctionGraph* FunctionGraphSet::findBy(QCPGraph* graph) const
{
    if (_graphT->contains(graph))
        return _graphT;
    if (_graphS->contains(graph))
        return _graphS;

    auto it = _graphs.constBegin();
    while (it != _graphs.constEnd())
    {
        if (it.value()->contains(graph))
            return it.value();
        it++;
    }

    return nullptr;
}
