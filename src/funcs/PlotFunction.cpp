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

//void PlotFunction::loadParams(QSettings *settings, const Elements& elems)
//{
//    FuncHelpers::openGroup(settings, alias());
//    FuncHelpers::loadVariable(settings, schema(), _arg, elems);
//}

//void PlotFunction::saveParams(QSettings *settings, bool canDeleteSettings)
//{
//    FuncHelpers::openGroup(settings, alias());
//    FuncHelpers::saveVariable(settings, schema(), _arg);
//    if (canDeleteSettings) delete settings;
//}

void PlotFunction::setError(const QString& error)
{
    if (!error.isEmpty()) Z_ERROR(error)
    _errorText = error;
}

bool PlotFunction::prepareResults(Z::PlottingRange range)
{
    _errorText.clear();
    _range.reset();
    Z_INFO(range.str());
    if (range.points() < 2)
    {
        setError(qApp->translate("Calc error", "Too few points for plotting"));
        clearResults();
        return false;
    }
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

//Element* PlotFunction::argElement()
//{
//    auto elem = _schema->element(_arg.element);
//    if (!elem)
//        setError(qApp->translate("Calc error",
//            "There is no element with index %1 in the schema").arg(_arg.element));
//    return elem;
//}

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
