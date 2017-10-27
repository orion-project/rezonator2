#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

#include "Values.h"
#include "Parameters.h"

class Element;

QT_BEGIN_NAMESPACE
//class QSettings;
QT_END_NAMESPACE

namespace Z {

/// Defines a range for plotting functions in SI-units.
class PlottingRange
{
private:
    double _start;
    double _stop;
    double _step;
    double _range;
    int _points;
    Unit _unit;
public:
    double start() const { return _start; } ///< Initial parameter value.
    double stop() const { return _stop; }   ///< Final parameter value.
    double step() const { return _step; }   ///< Parameter variation step.
    double range() const { return _range; } ///< Whole plotting range
    int points() const { return _points; }  ///< Amount of points.
    Unit unit() const { return _unit; }     ///< Unit of measurements for all values
    QString str() const;
    friend class VariableRange;
};

struct VariableRange
{
    Z::Value start;         ///< Initial parameter value.
    Z::Value stop;          ///< Final parameter value.
    Z::Value step;          ///< Parameter variation step.
    int points = 100;       ///< Amount of points.
    bool useStep = false;   ///< Use step value instead of points number.

    PlottingRange plottingRange() const;
};


struct Variable
{
    Element* element = nullptr;
    Z::Parameter* parameter = nullptr;
    VariableRange range;

/*REMOVE
//    int element;            ///< Element index.
//    int param;              ///< Parameter index.
//    double start;           ///< Initial parameter value.
//    double stop;            ///< Final parameter value.
//    double step;            ///< Parameter variation step.
//    int points;             ///< Amount of points.
//    bool useStep;           ///< Use step value instead of points number. */

    //Variable(): element(-1), param(-1), start(0), stop(0), step(0), points(1), useStep(true) {}

    //REMOVE double range() const { return stop - start; }

    /// Calculates points number and distance between points (step) to plotting a graph.
    //REMOVE void calculate(int& points, double& step);

//    void load(QSettings*);
//    void save(QSettings*);

//    QString str() const;
};

} // namespace Z

#endif // VARIABLE_H
