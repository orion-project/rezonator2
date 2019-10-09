#include "FunctionGraph.h"

#include "PlotFunction.h"
#include "../widgets/Plot.h"

//------------------------------------------------------------------------------
//                                 FunctionGraph
//------------------------------------------------------------------------------

FunctionGraph::FunctionGraph(Plot* plot, Z::WorkPlane workPlane, std::function<GraphUnits()> getUnits)
    : _plot(plot), _workPlane(workPlane), _getUnits(getUnits)
{
}

void FunctionGraph::clear()
{
    for (auto s : _segments)
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

QCPGraph* FunctionGraph::getOrMakeSegment(int index)
{
    QCPGraph *segment;
    if (index >= _segments.size())
    {
        segment = _plot->addGraph();
        segment->setPen(_linePen);
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

//------------------------------------------------------------------------------
//                               FunctionGraphSet
//------------------------------------------------------------------------------

FunctionGraphSet::FunctionGraphSet(Plot* plot, std::function<GraphUnits()> getUnits)
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
}

void FunctionGraphSet::clear()
{
    _graphT->clear();
    _graphS->clear();
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
