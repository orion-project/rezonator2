#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

#include "Parameters.h"

class Element;
class ElementRange;
class Schema;

namespace Z {

/**
    Defines a range for plotting functions in SI-units.
*/
class PlottingRange
{
private:
    double _start;
    double _stop;
    double _step;
    double _range;
    int _points;
    Unit _unit;
    QVector<double> _values;
public:
    double start() const { return _start; } ///< Initial parameter value.
    double stop() const { return _stop; }   ///< Final parameter value.
    double step() const { return _step; }   ///< Parameter variation step.
    double range() const { return _range; } ///< Whole plotting range.
    int points() const { return _points; }  ///< Amount of points.
    Unit unit() const { return _unit; }     ///< Unit of measurements for all values.
    const QVector<double>& values() const { return _values; } ///< Values of points.
    QString str() const;
    friend struct VariableRange;
};

/**
    Defines a range for plotting functions in arbitrary units.
*/
struct VariableRange
{
    Value start;            ///< Initial parameter value.
    Value stop;             ///< Final parameter value.
    Value step;             ///< Parameter variation step.
    int points = 100;       ///< Amount of points.
    bool useStep = false;   ///< Use step value instead of points number.

    /// Assigns only points number related values, but does not change start and stop.
    void assignPoints(const VariableRange& other);

    PlottingRange plottingRange() const;
    QString str() const;

    static VariableRange withPoints(const Value& start, const Value& stop, int points);
    static VariableRange withStep(const Value& start, const Value& stop, const Value& step);
};

/**
    Argument of plottings functions.
*/
struct Variable
{
    Element* element = nullptr;        ///< An element whose parameter value is varied.
    Parameter* parameter = nullptr;    ///< Element's parameter which value is varied.
    VariableRange range;               ///< Variation settings.

    QString str() const;
};


struct PlotPosition
{
    Element* element = nullptr;
    Value offset = 0;
};

namespace Utils {

/// The function returns the stop value of the range according to axis length of a range element.
Value getRangeStop(ElementRange *element);

} // namespace Utils

} // namespace Z

#endif // VARIABLE_H
