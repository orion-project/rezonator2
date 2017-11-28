#include "Calculator.h"
#include "PlotFunction.h"
#include "../core/Schema.h"
#include "../core/Protocol.h"

//------------------------------------------------------------------------------
//                                FunctionRange
//------------------------------------------------------------------------------

void FunctionRange::fit(const double& v)
{
    if (empty)
    {
        min = max = v;
        empty = false;
    }
    else
    {
        if (v < min) min = v;
        else if (v > max) max = v;
    }
}

QString FunctionRange::str() const
{
    return QString("[empty: %1; min: %2; max: %3]").arg(Z::str(empty), Z::str(min), Z::str(max));
}

//------------------------------------------------------------------------------
//                              PlotFuncResultSet
//------------------------------------------------------------------------------

void PlotFuncResultSet::reset()
{
    resultIndex = 0;
    results.resize(1);
    isBroken = false;
}

void PlotFuncResultSet::addPoint(double x, double y)
{
    if (std::isnan(y))
    {
        isBroken = true;
    }
    else
    {
        if (isBroken)
        {
           isBroken = false;
           results.resize(results.size()+1);
           resultIndex++;
           qDebug() << "new line started";
        }
        qDebug() << x << y;
        results[resultIndex].x.append(x);
        results[resultIndex].y.append(y);
    }
}

//------------------------------------------------------------------------------
//                                 PlotFunction
//------------------------------------------------------------------------------

PlotFunction::~PlotFunction()
{
    if (_calc) delete _calc;
}

bool PlotFunction::checkArguments()
{
    // These are not user involved errors, so translation is not required.
    if (!_arg.element)
    {
        setError("No variable element is set (PlotFunction.arg.element == null)");
        return false;
    }
    if (!_arg.parameter)
    {
        setError("No variable parameter is set (PlotFunction.arg.parameter == null)");
        return false;
    }
    return true;
}

void PlotFunction::setError(const QString& error)
{
    if (!error.isEmpty()) Z_ERROR(error)
    _errorText = error;
}

bool PlotFunction::prepareResults(Z::PlottingRange range)
{
    _errorText.clear();
    Z_INFO(range.str());
    if (range.points() < 2)
    {
        setError(qApp->translate("Calc error", "Too few points for plotting"));
        clearResults();
        return false;
    }
    _range.set(range.start(), range.stop());
    clearResults();
    return true;
}

void PlotFunction::clearResults()
{
    _resultsT.reset();
    _resultsS.reset();
}

bool PlotFunction::prepareCalculator(Element* ref, bool splitRange)
{
    if (_calc) delete _calc;

    _calc = new Calculator(_schema, ref);
    _calc->calcRoundTrip(splitRange);
    if (_calc->isEmpty())
    {
        setError(qApp->translate("Calc error", "Round trip is empty"));
        delete _calc;
        _calc = nullptr;
        return false;
    }
    return true;
}

void PlotFunction::addResultPoint(double x, double y_t, double y_s)
{
    _resultsT.addPoint(x, y_t);
    _resultsS.addPoint(x, y_s);
}

const PlotFuncResultSet* PlotFunction::results(Z::WorkPlane plane) const
{
    switch (plane)
    {
    case Z::Plane_T: return &_resultsT;
    case Z::Plane_S: return &_resultsS;
    }
    qCritical() << "Unknown Z::WorkPlane" << int(plane);
    return nullptr;
}

