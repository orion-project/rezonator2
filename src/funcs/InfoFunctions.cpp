#include "InfoFunctions.h"
#include "RoundTripCalculator.h"
#include "FormatInfo.h"
#include "../core/Format.h"
#include "../core/Schema.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                              InfoFuncMatrix

InfoFuncMatrix::InfoFuncMatrix(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem) {}

FunctionBase::FunctionState InfoFuncMatrix::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrix::calculate()
{
    return Z::Format::elementTitleAndMatrices(_element);
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrices

InfoFuncMatrices::InfoFuncMatrices(Schema *schema, const Elements& elems)
    : InfoFunction(schema), _elements(elems) {}

FunctionBase::FunctionState InfoFuncMatrices::elementDeleting(Element *elem)
{
    _elements.removeAll(elem);
    return _elements.isEmpty()? Dead: Ok;
}

QString InfoFuncMatrices::calculate()
{
    QString result;
    for (int i = 0; i < _elements.size(); i++)
    {
        result += Z::Format::elementTitleAndMatrices(_elements.at(i));
        if (i < _elements.size() - 1)
            result += "<hr>";
    }
    return result;
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrixMultFwd

InfoFuncMatrixMultFwd::InfoFuncMatrixMultFwd(Schema *schema, const Elements& elems)
    : InfoFuncMatrices(schema, elems) {}

QString InfoFuncMatrixMultFwd::calculate()
{
    Z::Matrix mt, ms;

    for (auto elem : _elements)
    {
        mt *= elem->Mt();
        ms *= elem->Ms();
    }

    return QStringLiteral("%1:<p>%2").arg(
                Z::Format::roundTrip(_elements, true),
                Z::Format::matrices(mt, ms));
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrixMultBkwd

InfoFuncMatrixMultBkwd::InfoFuncMatrixMultBkwd(Schema *schema, const Elements& elems)
    : InfoFuncMatrixMultFwd(schema, elems)
{
    Elements reversed;
    reversed.reserve(_elements.size());
    std::reverse_copy(_elements.begin(), _elements.end(), std::back_inserter(reversed));
    _elements = reversed;
}

//------------------------------------------------------------------------------
//                            InfoFuncMatrixRT

InfoFuncMatrixRT::InfoFuncMatrixRT(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem)
{
}

FunctionBase::FunctionState InfoFuncMatrixRT::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrixRT::calculate()
{
    RoundTripCalculator c(_schema, _element);
    c.calcRoundTrip();
    c.multMatrix();

    QString result = QStringLiteral("%1:<p>%2<hr><b>Ref.: </b>%3").arg(
                Z::Format::roundTrip(c.roundTrip(), true),
                Z::Format::matrices(c.Mt(), c.Ms()),
                Z::Format::linkViewMatrix(c.reference()));

    if (schema()->isResonator())
    {
        auto stab = c.stability();
        result += formatStability('T', stab.T) % formatStability('S', stab.S);
    }

    return result;
}

QString InfoFuncMatrixRT::formatStability(char plane, double value)
{
    return QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<nobr><b>P<sub>%1</sub></b> = %2</nobr>")
            .arg(plane).arg(Z::format(value));
}

//------------------------------------------------------------------------------
//                            InfoFuncRepetitionRate

QString InfoFuncRepetitionRate::calculate()
{
    _result = 0;
    double L = 0;
    int count = 0;
    QStringList parts;
    for (auto elem : schema()->elements())
    {
        auto range = Z::Utils::asRange(elem);
        if (range && !range->disabled())
        {
            count++;
            L += range->opticalPathSI();

            QString mult_ior;
            if (Double(range->ior()).isNot(1))
            {
                mult_ior = Z::Strs::multX() % Z::format(range->ior());
                count++; // elem should be printed even if it is single
            }
            parts += QString::fromLatin1("<b>%1</b>%2<font color=gray><i>(%3)</i></font>")
                .arg(Z::format(range->axisLengthSI()), mult_ior, range->displayLabel());
        }
    }

    if (Double(L).is(0))
        return qApp->translate("Func", "The schema does not contain elements having "
                "length (ranges, crystals, etc.), or their total length is zero.");

    _result = Z::Const::LightSpeed / L;
    if (schema()->isSW()) _result /= 2.0;

    Z::PrefixedValue freq(_result, Z::Units::Hz()); // convert to suitable kHz, MHz, etc.

    QString sum = "<b>" % Z::format(L) % Z::Units::m()->name() % "</b>";
    if (count > 1) sum = parts.join(" + ") % " = " % sum;

    return QString::fromLatin1("<p style='font-size:12pt'><b>%1</b><p>%2:<br>%3")
            .arg(freq.str(), qApp->translate("Func", "Total cavity length"), sum);
}

//------------------------------------------------------------------------------
//                             InfoFuncSummary

QString InfoFuncSummary::calculate()
{
    QStringList strs;
    for (Element* elem : schema()->elements())
        if (!elem->disabled() && elem->hasParams())
        {
            QStringList elemStrs;
            elemStrs << QStringLiteral("<font color=maroon>%1</font>").arg(elem->displayLabel());
            if (elem->hasParams())
            {
                elemStrs << ":";
                elemStrs << Z::Format::elemParamsHtml(schema(), elem, false);
            }
            if (!elem->title().isEmpty())
                elemStrs << QStringLiteral("<i>(%1)</i>").arg(elem->title());
            strs << elemStrs.join(' ');
        }

    if (!strs.isEmpty())
        strs << "";
    strs << Z::Format::paramHtml(&schema()->wavelength());

    if (schema()->isSP())
    {
        auto pump = schema()->activePump();
        if (pump)
        {
            strs << "";
            strs << qApp->translate("Func", "<b>Input beam:</b>");
            auto pumpMode = Z::Pump::findByModeName(pump->modeName());
            if (pumpMode) strs << pumpMode->description();
            strs << Z::Format::pumpParamsHtml(pump);
        }
    }

    return strs.join("<br>");
}

