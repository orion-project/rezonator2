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
    _x_t = QVector<double>(range.points()), _y_t = QVector<double>(range.points());
    _x_s = QVector<double>(range.points()), _y_s = QVector<double>(range.points());
    return true;
}

void PlotFunction::clearResults()
{
    _x_t.clear(); _x_s.clear();
    _y_t.clear(); _y_s.clear();
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

//ElementRange* PlotFunction::argElementRange()
//{
//    auto elem = argElement();
//    if (!elem)
//        return nullptr;
//    auto range = Z::Utils::asRange(elem);
//    if (!range)
//        setError(qApp->translate("Calc error",
//            "Element %1 is not a range").arg(elem->displayLabel()));
//    return range;
//}
