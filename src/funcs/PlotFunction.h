#ifndef PLOT_FUNCTION_H
#define PLOT_FUNCTION_H

#include <QVector>

#include "FunctionBase.h"
#include "../core/Variable.h"

class Calculator;
class Schema;

/**
    Backups and restores parameter value, and prevents multiple elementChanged events.
*/
class BackupAndLock
{
public:
    BackupAndLock(Element* elem, Z::Parameter* param): _elem(elem), _param(param), _backup(param->value()) { _elem->lock();}
    ~BackupAndLock() { _param->setValue(_backup); _elem->unlock(); }
private:
    Element* _elem;
    Z::Parameter* _param;
    Z::Value _backup;
};

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

struct PlotFuncResult
{
    QVector<double> x, y;

    void clear() { x.clear(); y.clear(); }
    void append(double ax, double ay) { x.append(ax); y.append(ay); }
};

struct PlotFuncResultSet
{
    QString id;
    QVector<PlotFuncResult> results;
    int resultIndex;
    bool isSegmentEnded;
    bool makeNewSegment;

    void reset();
    void addPoint(double x, double y);
    int allPointsCount() const;
};

/**
    Base class for all plotting functions.
    Plotting function is a function presenting its calculation results in graphical form.
*/
class PlotFunction : public FunctionBase
{
public:
    PlotFunction(Schema *schema);
    ~PlotFunction();

    virtual bool checkArguments();

    virtual void calculate() {}

    /// Defines if function can calculate notable values. See @ref calculateNotables().
    virtual bool hasNotables() const { return false; }

    /// Calculate some notable values.
    /// E.g. for stability maps these values are stability boundaries,
    /// for caustic these are waist radius and position.
    virtual QString calculateNotables() { return QString(); }

    virtual QString calculatePoint(const double&) { return QString(); }

    /// Defines if function can show its result as data table.
    virtual bool hasDataTable() const { return true; }

    /// Defines if function has additional params besides of variable argument.
    virtual bool hasOptions() const { return false; }

    /// Returns calculated results count for specific workplane - T or S
    /// Function can calculate more than one result if plot is splitted into segments.
    /// For example plot of curvature radius of caustic function has a pole at waist,
    /// where it changes its value from positive to negative infinity or vice versa.
    /// At this point, one plot segment will be ended and a new one will be started.
    /// See @ref PlotFuncResultSet::addPoint()
    virtual int resultCount(Z::WorkPlane plane) const { return results(plane)->results.size(); }

    /// Returns calculated results for specific workplane - T or S
    virtual const PlotFuncResult& result(Z::WorkPlane plane, int index) const { return results(plane)->results.at(index); }

    bool ok() const { return _errorText.isEmpty(); }
    const QString& errorText() const { return _errorText; }

    Z::Variable* arg() { return &_arg; }
    const FunctionRange& range() const { return _range; }

protected:
    Z::Variable _arg;
    Calculator* _calc = nullptr;
    Z::ValueTS<PlotFuncResultSet> _results;
    FunctionRange _range;
    Z::Value _backupValue;

    void setError(const QString& error);

    void clearResults();
    bool prepareResults(Z::PlottingRange range);
    void finishResults();
    void addResultPoint(double x, double y_t, double y_s);
    void addResultPoint(double x, const Z::PointTS& point) { addResultPoint(x, point.T, point.S); }
    bool prepareCalculator(Element* ref, bool splitRange = false);
    const PlotFuncResultSet *results(Z::WorkPlane plane) const;

private:
    QString _errorText;
};

#endif // PLOT_FUNCTION_H
