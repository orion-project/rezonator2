#ifndef FUNCTION_GRAPH_H
#define FUNCTION_GRAPH_H

#include "../core/CommonTypes.h"
#include "../core/Units.h"

#include <QVector>
#include <QPen>

class QCPGraph;
class Plot;
class PlotFunction;

class FunctionGraph
{
public:
    FunctionGraph(Plot* plot, Z::WorkPlane workPlane);

    void clear();
    void update(PlotFunction* function);
    void update(const QList<PlotFunction*>& functions);

    int segmentsCount() const { return _segments.size(); }

    bool isFlipped() const { return _isFlipped; }
    void setFlipped(bool on) { _isFlipped = on; }
    bool isVisible() const { return _isVisible; }
    void setVisible(bool on) { _isVisible = on; }

    void setPen(const QPen& pen) { _linePen = pen; }

    std::function<Z::Unit()> getUnitX;
    std::function<Z::Unit()> getUnitY;

private:
    Plot* _plot;
    Z::WorkPlane _workPlane;
    bool _isFlipped = false;
    bool _isVisible = true;
    QVector<QCPGraph*> _segments;
    QPen _linePen;

    QCPGraph* getOrMakeSegment(int index);
    void fillSegment(QCPGraph* segment, PlotFunction* function, int resultIndex, double offset = 0);
    void trimToCount(int count);
};

#endif // FUNCTION_GRAPH_H
