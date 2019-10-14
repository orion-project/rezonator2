#include "PlotFuncRoundTripFunction.h"

#include "FormatInfo.h"
#include "PlotFunction.h"
#include "RoundTripCalculator.h"

#include <QTimer>

PlotFuncRoundTripFunction::PlotFuncRoundTripFunction(const QString &funcTitle, PlotFunction* func)
    : InfoFunction(func->schema()), _funcTitle(funcTitle), _function(func)
{
    _function->registerListener(this);
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

    QString result;
    QTextStream stream(&result);
    stream << Z::Format::roundTrip(c->roundTrip(), true) << QChar(':')
           << Z::Format::matrices(c->Mt(), c->Ms())
           << QStringLiteral("<br><span class=param>Ref:&nbsp;</span>")
           << Z::Format::linkViewMatrix(c->reference());

    auto matrsT = c->matrsT();
    auto matrsS = c->matrsS();
    auto owners = c->matrixOwners();
    int count = matrsT.size();
    for (int i = 0; i < count; i++)
    {
        auto elem = owners.at(i);

        stream << QStringLiteral("<hr>");

        stream << QStringLiteral("<span class=elem_label>")
               << elem->displayLabel()
               << QStringLiteral("</span>");

        if (!elem->title().isEmpty())
            stream << QStringLiteral(" <span class=elem_title>(")
                   << elem->title()
                   << QStringLiteral(")</span>");

        stream << Z::Format::matrices(matrsT.at(i), matrsS.at(i));
    }

    return result;
}
