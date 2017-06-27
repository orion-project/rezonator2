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

struct VariableRange
{
    Z::Value start;         ///< Initial parameter value.
    Z::Value stop;          ///< Final parameter value.
    Z::Value step;          ///< Parameter variation step.
    int points = 100;       ///< Amount of points.
    bool useStep = false;   ///< Use step value instead of points number.
};


struct Variable
{
    Element* element = nullptr;
    Z::Parameter* parameter = nullptr;
    VariableRange range;

//    int element;            ///< Element index.
//    int param;              ///< Parameter index.
//    double start;           ///< Initial parameter value.
//    double stop;            ///< Final parameter value.
//    double step;            ///< Parameter variation step.
//    int points;             ///< Amount of points.
//    bool useStep;           ///< Use step value instead of points number.

    //Variable(): element(-1), param(-1), start(0), stop(0), step(0), points(1), useStep(true) {}

    //double range() const { return stop - start; }

    /// Calculates points number and distance between points (step) to plotting a graph.
    void calculate(int& points, double& step);

//    void load(QSettings*);
//    void save(QSettings*);

//    QString str() const;
};

} // namespace Z

#endif // VARIABLE_H
