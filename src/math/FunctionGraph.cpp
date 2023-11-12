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
        if (index == 0)
        {
            segment->addToLegend();
            if (legendName.isEmpty())
                segment->setName(Z::planeName(_workPlane));
            else segment->setName(legendName);
        }
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

void FunctionGraph::setColor(const QString& color)
{
    QColor clr(color);
    if (!clr.isValid())
        clr = _workPlane == Z::T ? Qt::darkGreen : Qt::red;
    setPen(QPen(clr));
}

bool FunctionGraph::contains(QCPGraph* graph) const
{
    foreach (auto g, segments())
        if (graph == g) return true;
    return false;
}

QString FunctionGraph::str() const
{
    QString s;
    QTextStream res(&s);
    auto units = _getUnits();
    for (int i = 0; i < _segments.size(); i++)
    {
        auto g = _segments.at(i);
        if (segments().size() > 1)
            res << "segment " << i << '\n';
        res << units.X->alias() << '\t' << units.Y->alias() << '\n';
        auto data = g->data().data();
        auto it = data->constBegin();
        while (it != data->constEnd())
        {
            res << it->key << '\t' << it->value << '\n';
            it++;
        }
    }
    return s;
}

FunctionGraph::ExportData FunctionGraph::exportData(ExportParams params) const
{
    QVector<Z::DoublePoint> d;
    for (int i = 0; i < _segments.size(); i++)
    {
        if (params.segmentIdx >= 0 && params.segmentIdx != i)
            continue;
        auto g = _segments.at(i);
        auto data = g->data().data();
        auto it = data->constBegin();
        while (it != data->constEnd())
        {
            d << Z::DoublePoint{it->key, it->value};
            it++;
        }
    }
    return d;
}

//------------------------------------------------------------------------------
//                               FunctionGraphSet
//------------------------------------------------------------------------------

FunctionGraphSet::FunctionGraphSet(QCPL::Plot* plot, std::function<GraphUnits()> getUnits): _plot(plot), _getUnits(getUnits)
{
    _graphT = new FunctionGraph(plot, Z::T, getUnits);
    _graphS = new FunctionGraph(plot, Z::S, getUnits);
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
    QString key = id + Z::planeSuffix(workPlane);
    if (!_graphs.contains(key))
    {
        auto graph = new FunctionGraph(_plot, workPlane, _getUnits);
        graph->id = id;
        graph->legendName = key;
        graph->setColor(color);
        _graphs.insert(key, graph);
    }
    else
        _graphs[key]->setColor(color);
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

QString FunctionGraphSet::str() const
{
    QString report;
    QTextStream res(&report);
    {
        res << "graph_t\n";
        QString s = _graphT->str();
        if (s.isEmpty())
            res << "(none)\n";
        else res << s;
    }
    if (!report.isEmpty())
        res << '\n';
    {
        res << "graph_s\n";
        QString s = _graphS->str();
        if (s.isEmpty())
            res << "(none)\n";
        else res << s;
    }
    auto it = _graphs.constBegin();
    while (it != _graphs.constEnd())
    {
        if (!report.isEmpty())
            res << '\n';
        res << it.key() << '\n';
        QString s = it.value()->str();
        if(s.isEmpty())
            res << "(none)\n";
        else res << s;
        it++;
    }
    return report;
}

FunctionGraphSet::ExportData FunctionGraphSet::exportData(ExportParams params) const
{
    ExportData dd;

    auto addColumn = [&dd](const QString& col, const FunctionGraph::ExportData& d) {
        int c = dd.cols.size();
        dd.cols << col;
        foreach (const auto& p, d)
        {
            if (dd.data.contains(p.X))
                dd.data[p.X][c] = p.Y;
            else dd.data[p.X] = {{c, p.Y}};
        }
    };

    // Currently it either can be TS-graph or multigraph (e.g. MR-caustic), but not both
    if (_graphT->isEmpty() && _graphS->isEmpty())
    {
        int segmentIdx = -1;
        if (params.segment) {
            // We don't know on which line the selected segment is,
            // but we want to export respective segments from all lines
            auto it = _graphs.constBegin();
            while (it != _graphs.constEnd()) {
                int i = it.value()->segments().indexOf(params.segment);
                if (i > -1) {
                    segmentIdx = i;
                    break;
                }
                it++;
            }
        }

        auto it = _graphs.constBegin();
        while (it != _graphs.constEnd())
        {
            if (!params.graph || it.value()->segments().contains(params.graph))
                addColumn(it.key(), it.value()->exportData(FunctionGraph::ExportParams{.segmentIdx = segmentIdx}));
            it++;
        }
    }
    else
    {
        // We don't know on which T or S line the selected segment is,
        // but we want to export respective segments from both lines
        int segmentIdx = _graphT->segments().indexOf(params.segment);
        if (segmentIdx == -1)
            segmentIdx = _graphS->segments().indexOf(params.segment);
        if (params.useT)
            addColumn("t", _graphT->exportData(FunctionGraph::ExportParams{.segmentIdx = segmentIdx}));
        if (params.useS)
            addColumn("s", _graphS->exportData(FunctionGraph::ExportParams{.segmentIdx = segmentIdx}));
    }
    return dd;
}

QString FunctionGraphSet::ExportData::str() const
{
    QString report;
    QTextStream res(&report);
    res << "x";
    foreach (const auto& c, cols)
        res << '\t' << c;
    res << '\n';
    int colCount = cols.size();
    auto it = data.constBegin();
    while (it != data.constEnd())
    {
        double x = it.key();
        const auto& y = data[x];
        res << x;
        for (int c = 0; c < colCount; c++)
            res << '\t' << (y.contains(c) ? y[c] : ' ');
        res << '\n';
        it++;
    }
    return report;
}
