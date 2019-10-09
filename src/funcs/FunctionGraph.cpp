#include "FunctionGraph.h"

#include "PlotFunction.h"
#include "../widgets/Plot.h"

FunctionGraph::FunctionGraph(Plot* plot, Z::WorkPlane workPlane): _plot(plot), _workPlane(workPlane)
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

void FunctionGraph::fillSegment(QCPGraph* segment, PlotFunction* function, int resultIndex, double offset)
{
    auto result = function->result(_workPlane, resultIndex);
    int count = result.pointsCount();
    auto xs = result.x();
    auto ys = result.y();
    auto unitX = getUnitX();
    auto unitY = getUnitY();
    QSharedPointer<QCPGraphDataContainer> data(new QCPGraphDataContainer);
    for (int i = 0; i < count; i++)
    {
        // TODO: possible optimization: extract unit's SI factor before loop
        // and replace the call of virtual method with simple multiplication
        double x = unitX->fromSi(xs.at(i) + offset);
        double y = unitY->fromSi(ys.at(i)) * (_isFlipped ? -1 : 1);
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
