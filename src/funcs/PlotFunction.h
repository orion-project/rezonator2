#ifndef PLOT_FUNCTION_H
#define PLOT_FUNCTION_H

#include <QVector>

#include "FunctionBase.h"
#include "../core/Variable.h"

class Calculator;
class Schema;

class FunctionRange
{
public:
    double min = 0;
    double max = 0;
    bool empty = true;
    void reset() { empty = true; min = max = 0; }
    void set(const double& v1, const double& v2) { empty = false, min = v1, max = v2; }
    bool has(const double& v) const { return !empty && v >= min && v <= max; }
    void fit(const double& v);
    QString str() const;
};

/**
    Base class for all plotting functions.
    Plotting function is a function presenting its calculation results in graphical form.
*/
class PlotFunction : public FunctionBase
{
public:
    PlotFunction(Schema *schema) : FunctionBase(schema) {}
    ~PlotFunction();

//    virtual void loadParams(QSettings*, const Elements&);
//    virtual void saveParams(QSettings*, bool canDeleteSettings = false);

    virtual void calculate() {}
    virtual QString calculateNotables() { return QString(); }
    virtual QString calculatePoint(const double&) { return QString(); }

    virtual int resultCount(Z::WorkPlane) const { return 1; }
    virtual const QVector<double>& resultX(Z::WorkPlane plane, int) const { return plane == Z::Plane_T? _x_t: _x_s; }
    virtual const QVector<double>& resultY(Z::WorkPlane plane, int) const { return plane == Z::Plane_T? _y_t: _y_s; }

    bool ok() const { return _errorText.isEmpty(); }
    const QString& errorText() const { return _errorText; }

    Z::Variable* arg() { return &_arg; }
    const FunctionRange& range() const { return _range; }

protected:
    Z::Variable _arg;
    Calculator* _calc = nullptr;
    QVector<double> _x_t, _y_t, _x_s, _y_s;
    FunctionRange _range;

    void setError(const QString& error);

    void clearResults();
    bool prepareResults(Z::PlottingRange range);
    bool prepareCalculator(Element* ref, bool splitRange = false);

//    Element* argElement();
//    ElementRange* argElementRange();

private:
    QString _errorText;
};

#endif // PLOT_FUNCTION_H
