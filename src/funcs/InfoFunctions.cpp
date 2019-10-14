#include "InfoFunctions.h"

#include "FormatInfo.h"
#include "RoundTripCalculator.h"
#include "../Appearance.h"
#include "../core/Format.h"
#include "../core/Schema.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                              InfoFuncMatrix
//------------------------------------------------------------------------------

InfoFuncMatrix::InfoFuncMatrix(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem) {}

FunctionBase::FunctionState InfoFuncMatrix::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrix::calculateInternal()
{
    return Z::Format::elementTitleAndMatrices(_element);
}

//------------------------------------------------------------------------------
//                             InfoFuncMatrices
//------------------------------------------------------------------------------

InfoFuncMatrices::InfoFuncMatrices(Schema *schema, const Elements& elems)
    : InfoFunction(schema), _elements(elems) {}

FunctionBase::FunctionState InfoFuncMatrices::elementDeleting(Element *elem)
{
    _elements.removeAll(elem);
    return _elements.isEmpty()? Dead: Ok;
}

QString InfoFuncMatrices::calculateInternal()
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
//------------------------------------------------------------------------------

InfoFuncMatrixMultFwd::InfoFuncMatrixMultFwd(Schema *schema, const Elements& elems)
    : InfoFuncMatrices(schema, elems) {}

QString InfoFuncMatrixMultFwd::calculateInternal()
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
//------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------

InfoFuncMatrixRT::InfoFuncMatrixRT(Schema *schema, Element *elem)
    : InfoFunction(schema), _element(elem)
{
}

FunctionBase::FunctionState InfoFuncMatrixRT::elementDeleting(Element *elem)
{
    if (_schema->count() == 1) return Dead;
    return _element == elem? Frozen: Ok;
}

QString InfoFuncMatrixRT::calculateInternal()
{
    RoundTripCalculator c(_schema, _element);
    c.calcRoundTrip();
    c.multMatrix();

    QStringList report;
    report << Z::Format::roundTrip(c.roundTrip(), true) << QChar(':')
           << Z::Format::matrices(c.Mt(), c.Ms())
           << QStringLiteral("<hr><span class=param>Ref:&nbsp;</span>")
           << Z::Format::linkViewMatrix(c.reference());

    if (schema()->isResonator())
    {
        auto stab = c.stability();
        report << formatStability('T', stab.T)
               << formatStability('S', stab.S);
    }

    return report.join(QString());
}

QString InfoFuncMatrixRT::formatStability(char plane, double value)
{
    return QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "<span class=param>P<sub>%1</sub></span>"
                          "<span class=value> = %2</span>")
            .arg(plane).arg(Z::format(value));
}

//------------------------------------------------------------------------------
//                            InfoFuncRepetitionRate
//------------------------------------------------------------------------------

QString InfoFuncRepetitionRate::calculateInternal()
{
    _repetitonRate = 0;
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
                count++; // when n != 1 then the sum should be printed even if it is the only element
            }
            parts += QString::fromLatin1("<b>%1</b>%2<font color=gray><i>(%3)</i></font>")
                .arg(Z::format(range->axisLengthSI()), mult_ior, range->displayLabel());
        }
    }

    if (Double(L).is(0))
        return qApp->translate("Func", "The schema does not contain elements having "
                "length (ranges, crystals, etc.), or their total length is zero.");

    _repetitonRate = Z::Const::LightSpeed / L;
    if (schema()->isSW()) _repetitonRate /= 2.0;

    Z::PrefixedValue freq(_repetitonRate, Z::Units::Hz()); // convert to suitable kHz, MHz, etc.

    QString sum = "<b>" % Z::format(L) % Z::Units::m()->name() % "</b>";
    if (count > 1) sum = parts.join(" + ") % " = " % sum;

#ifdef Q_OS_MAC
    const int freqFontSize = 16;
#else
    const int freqFontSize = 12;
#endif
    return QStringLiteral("<p style='font-size:%1pt'><b>%2</b>"
                          "<p>%3:<br>%4")
            .arg(freqFontSize)
            .arg(freq.format())
            .arg(qApp->translate("Func", "Total cavity length"))
            .arg(sum);
}

//------------------------------------------------------------------------------
//                             InfoFuncSummary
//------------------------------------------------------------------------------

QString InfoFuncSummary::calculateInternal()
{
    Z::Format::FormatElemParams formatParams;
    formatParams.schema = schema();

    QStringList report;
    report << QStringLiteral("<table border=1 cellspacing=-1 style='border-color:#aaa;border-style:solid'>");
    for (Element* elem : schema()->elements())
        if (!elem->disabled() && elem->hasParams())
        {
            report << QStringLiteral("<tr><td style='padding:2 4'><span style='")
                   << Z::Gui::html(Z::Gui::ElemLabelFont())
                   << QStringLiteral("'>")
                   << elem->displayLabel()

                   // weird, but the space after span is important!
                   // without it vertical alignment calculated improperly sometimes
                   << QStringLiteral("</span> </td><td style='padding:2 4'>")

                   << formatParams.format(elem)
                   << QStringLiteral("</td><td style='padding:2 4'>")
                   << elem->title()
                   << QStringLiteral("</td></tr>");
        }
    report << QStringLiteral("</table><br>");

    Z::Format::FormatParam formatWavelength;
    formatWavelength.includeDriver = false;
    formatWavelength.includeValue = true;
    report << QStringLiteral("<br>")
           << formatWavelength.format(&schema()->wavelength())
           << QStringLiteral("<br>");

    if (schema()->isSP())
    {
        auto pump = schema()->activePump();
        if (pump)
        {
            report << QStringLiteral("<br>")
                   << qApp->translate("InfoFuncSummary", "<b>Input beam:</b>")
                   << QStringLiteral("<br>");

            auto pumpMode = Pump::findByModeName(pump->modeName());
            if (pumpMode)
                report << pumpMode->description()
                       << QStringLiteral("<br>");

            report << Z::Format::FormatPumpParams().format(pump);
        }
    }

    return report.join(QString());
}
