#include "PlotFuncRoundTripFunction.h"

#include "FormatInfo.h"
#include "PlotFunction.h"
#include "RoundTripCalculator.h"
#include "../AppSettings.h"

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

    if (c->splitRange())
    {
        auto range = Z::Utils::asRange(c->reference());
        if (range)
            stream << " (offset inside: " << Z::Utils::getSubRangeLf(range).displayStr() << ")";
    }

    QString resultPy;
    QTextStream streamPy(&resultPy);
    QStringList roundTripT, roundTripS;

    auto matrsT = c->matrsT();
    auto matrsS = c->matrsS();
    auto info = c->matrixInfo();
    int count = matrsT.size();
    for (int i = 0; i < count; i++)
    {
        auto mi = info.at(i);

        stream << QStringLiteral("<hr>");

        stream << QStringLiteral("<span class=elem_label>")
               << mi.owner->displayLabel()
               << QStringLiteral("</span>");

        if (!mi.owner->title().isEmpty())
            stream << QStringLiteral(" <span class=elem_title>(")
                   << mi.owner->title()
                   << QStringLiteral(")</span>");

        if (mi.kind == mi.LEFT_HALF)
        {
            auto range = Z::Utils::asRange(mi.owner);
            if (range)
                stream << " (left half: " << Z::Utils::getSubRangeLf(range).displayStr() << ")";
        }
        else if (mi.kind == mi.RIGHT_HALF)
        {
            auto range = Z::Utils::asRange(mi.owner);
            if (range)
                stream << " (right half: " << Z::Utils::getSubRangeRt(range).displayStr() << ")";
        }
        else if (mi.kind == mi.BACK_PASS)
            stream << " (back pass)";

        stream << Z::Format::matrices(matrsT.at(i), matrsS.at(i));

        if (AppSettings::instance().showPythonMatrices)
        {
            QString nameT = Z::Format::Py::matrixVarName(mi.owner, "_t");
            QString nameS = Z::Format::Py::matrixVarName(mi.owner, "_s");
            if (mi.kind == mi.LEFT_HALF)
            {
                nameT += "_lf";
                nameS += "_lf";
            }
            else if (mi.kind == mi.RIGHT_HALF)
            {
                nameT += "_rt";
                nameS += "_rt";
            }
            else if (mi.kind == mi.BACK_PASS)
            {
                nameT += "_inv";
                nameS += "_inv";
            }
            roundTripT << nameT;
            roundTripS << nameS;
            stream << "<p><code>"
                   << Z::Format::Py::matrixAsNumpy(nameT, matrsT.at(i)) << "<br>"
                   << Z::Format::Py::matrixAsNumpy(nameS, matrsS.at(i))
                   << "</code></p>";
            streamPy
                   << Z::Format::Py::matrixAsNumpy(nameT, matrsT.at(i)) << "<br>"
                   << Z::Format::Py::matrixAsNumpy(nameS, matrsS.at(i)) << "<br><br>";
        }
    }
    if (AppSettings::instance().showPythonMatrices)
    {
        streamPy << "# Round-trip matrices<br>"
                 << "M0_t = " << roundTripT.join(" * ") << "<br>"
                 << "M0_s = " << roundTripS.join(" * ") << "<br>";

        stream << "<hr><p><code>" << resultPy << "</code>";
    }

    return result;
}
