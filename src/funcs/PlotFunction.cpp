#include "RoundTripCalculator.h"
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
    results[0].clear();
    isSegmentEnded = false;
    makeNewSegment = false;
}

void PlotFuncResultSet::addPoint(double x, double y)
{
    if (std::isnan(y) || std::isinf(y))
    {
        int segmentLen = results[resultIndex].x.size();
        if (!isSegmentEnded && segmentLen > 0)
        {
            Z_INFO(id << "line segment ended at" << x << "points count:" << segmentLen);
            isSegmentEnded = true;
            if (segmentLen < 2)
            {
                Z_INFO(id << "segment is too short, skipped");
                results[resultIndex].clear();
                makeNewSegment = false;
            }
            else makeNewSegment = true;
        }
    }
    else
    {
        if (isSegmentEnded)
        {
            isSegmentEnded = false;
            if (makeNewSegment)
            {
                results.resize(results.size()+1);
                resultIndex++;
            }
            Z_INFO(id << "line continued with new segment at" << x)
        }
        //Z_INFO(id << "point" << x << y);
        results[resultIndex].append(x, y);
    }
}

int PlotFuncResultSet::allPointsCount() const
{
    int count = 0;
    for (const PlotFuncResult& result : results)
        count += result.x.size();
    return count;
}

//------------------------------------------------------------------------------
//                                 PlotFunction
//------------------------------------------------------------------------------

PlotFunction::PlotFunction(Schema *schema) : FunctionBase(schema)
{
    _results.T.id = QStringLiteral("T");
    _results.S.id = QStringLiteral("S");
}

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

void PlotFunction::clearResults()
{
    _results.T.reset();
    _results.S.reset();
}

bool PlotFunction::prepareResults(Z::PlottingRange range)
{
    Z_REPORT("Calc:" << name());
    Z_INFO(range.str());
    _errorText.clear();
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

void PlotFunction::finishResults()
{
    int totalCountT = _results.T.allPointsCount();
    int totalCountS = _results.S.allPointsCount();
    Z_INFO("Total points count: T =" << totalCountT << "S =" << totalCountS);
    if (totalCountT == 0 && totalCountS == 0)
        setError(qApp->translate("Calc error", "No one valid point was calculated"));
}

bool PlotFunction::prepareCalculator(Element* ref, bool splitRange)
{
    if (_calc) delete _calc;

    _calc = new RoundTripCalculator(_schema, ref);
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
    _results.T.addPoint(x, y_t);
    _results.S.addPoint(x, y_s);
}

const PlotFuncResultSet* PlotFunction::results(Z::WorkPlane plane) const
{
    switch (plane)
    {
    case Z::Plane_T: return &_results.T;
    case Z::Plane_S: return &_results.S;
    }
    qCritical() << "Unknown Z::WorkPlane" << int(plane);
    return nullptr;
}

