#include "PlotFuncRoundTripFunction.h"

#include "InfoFunctions.h"
#include "PlotFunction.h"
#include "RoundTripCalculator.h"
#include "../AppSettings.h"
#include "../core/Schema.h"

#include <QTimer>

PlotFuncRoundTripFunction::PlotFuncRoundTripFunction(const QString &funcTitle, PlotFunction* func)
    : InfoFunction(func->schema()), _funcTitle(funcTitle), _function(func)
{
    _function->registerListener(this);

    _actions << InfoFuncAction{
        .title = qApp->translate("PlotFuncRoundTripFunction", "Show all element matrices"),
        .icon = ":/toolbar/elem_matr",
        .triggered = [this](){ _showElems = !_showElems; calculate(); },
        .isChecked = [this](){ return _showElems; },
    };
}

PlotFuncRoundTripFunction::~PlotFuncRoundTripFunction()
{
    if (_function)
        _function->unregisterListener(this);
}

void PlotFuncRoundTripFunction::functionCalculated(FunctionBase*)
{
    calculate();
}

void PlotFuncRoundTripFunction::functionDeleted(FunctionBase*)
{
    _function = nullptr;
    QTimer::singleShot(0, [this]{ delete this; });
}

QString PlotFuncRoundTripFunction::calculateInternal()
{
    auto c = _function->roundTripCalculator();
    if (!c)
        return "There is no round-trip calculated for the function.";

    return InfoFuncMatrixRT::format(c, _showElems);
}
