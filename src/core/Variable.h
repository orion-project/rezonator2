#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

#include "Parameters.h"

class Element;
class Schema;

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

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
    Z::Value start;         ///< Initial parameter value.
    Z::Value stop;          ///< Final parameter value.
    Z::Value step;          ///< Parameter variation step.
    int points = 100;       ///< Amount of points.
    bool useStep = false;   ///< Use step value instead of points number.

    PlottingRange plottingRange() const;
    QString str() const;
};

/**
    Argument of plottings functions.
*/
struct Variable
{
    Element* element = nullptr;        ///< An element whose parameter value is varied.
    Z::Parameter* parameter = nullptr; ///< Element's parameter which value is varied.
    VariableRange range;               ///< Variation settings.

    void load(QSettings*, Schema*);
    void save(QSettings*);

    QString str() const;
};

} // namespace Z

#endif // VARIABLE_H
