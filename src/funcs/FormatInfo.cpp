#include "FormatInfo.h"

#include "../Appearance.h"
#include "../core/Element.h"
#include "../core/Format.h"
#include "../core/Schema.h"

#include <QApplication>
#include <QFont>

using namespace Z::Gui;

namespace Z {
namespace Format {

QString matrix(const Z::Matrix& m)
{
    bool real = m.isReal();
    return QStringLiteral(
        "<table border=1 cellpadding=5 cellspacing=-1 style='border-color:gray;border-style:solid'>"
            "<tr>"
                "<td align=right><pre class=value>%1</pre></td>"
                "<td align=right><pre class=value>%2</pre></td>"
            "</tr>"
            "<tr>"
                "<td align=right><pre class=value>%3</pre></td>"
                "<td align=right><pre class=value>%4</pre></td>"
            "</tr>"
        "</table>"
    ).arg(real ? Z::format(m.A.real()) : Z::format(m.A),
          real ? Z::format(m.B.real()) : Z::format(m.B),
          real ? Z::format(m.C.real()) : Z::format(m.C),
          real ? Z::format(m.D.real()) : Z::format(m.D));
}

QString matrices(const Z::Matrix& mt, const Z::Matrix& ms)
{
    return QStringLiteral(
        "<table cellpadding=3 valign=middle style='margin-top:5px'>"
            "<tr>"
                "<td><pre class=param>M<sub>T</sub>&nbsp;=</pre></td>"
                "<td>%1</td>"
                "<td style='padding-left:10px'><pre class=param>M<sub>S</sub>&nbsp;=</pre></td>"
                "<td>%2</td>"
                "<td style='padding-left:10px'>"
                    "<pre><span class=param>&Delta;<sub>T</sub></span><span class=value> = %3</span></pre>"
                    "<pre><span class=param>&Delta;<sub>S</sub></span><span class=value> = %4</span></pre>"
                "</td>"
            "</tr>"
        "</table>"
    ).arg(matrix(mt),
          matrix(ms),
          mt.isReal() ? Z::format(mt.det().real()) : Z::format(mt.det()),
          ms.isReal() ? Z::format(ms.det().real()) : Z::format(ms.det()));
}

QString matrix(const QString& label, const Z::Matrix& m)
{
    return QStringLiteral(
        "<table cellpadding=3 valign=middle style='margin-top:5px'>"
            "<tr>"
                "<td><pre class=param>%1&nbsp;=</pre></td>"
                "<td>%2</td>"
            "</tr>"
        "</table>"
    ).arg(label, matrix(m));
}

namespace Py {
QString formatPy(const Z::Complex& v)
{
    return QString("%1%2%3j")
            .arg(v.real(), 0, 'g', 16)
            .arg(v.imag() < 0 ? '-' : '+')
            .arg(v.imag(), 0, 'g', 16);
}

QString matrixAsNumpy(const QString& label, const Z::Matrix& m)
{
    bool real = m.isReal();
    return QStringLiteral("%1 = np.matrix([[%2, %3], [%4, %5]])")
        .arg(label)
        .arg(real ? Z::str(m.A.real()) : formatPy(m.A))
        .arg(real ? Z::str(m.B.real()) : formatPy(m.B))
        .arg(real ? Z::str(m.C.real()) : formatPy(m.C))
        .arg(real ? Z::str(m.D.real()) : formatPy(m.D));
}

QString matrixVarName(Element *elem, const QString& suffix)
{
    return QStringLiteral("M_%1%2").arg(elem->displayLabel()).arg(suffix);
}

QString elementMatrices(Element *elem)
{
    if (elem->hasOption(Element_Asymmetrical))
        return QStringLiteral("<p><code>%1</code><br><code>%2</code><br><code>%3</code><br><code>%4</code></p>")
            .arg(matrixAsNumpy(matrixVarName(elem, "_f_t"), elem->Mt()))
            .arg(matrixAsNumpy(matrixVarName(elem, "_f_s"), elem->Ms()))
            .arg(matrixAsNumpy(matrixVarName(elem, "_b_t"), elem->Mt_inv()))
            .arg(matrixAsNumpy(matrixVarName(elem, "_b_s"), elem->Ms_inv()));
    return QStringLiteral("<p><code>%1</code><br><code>%2</code></p>")
        .arg(matrixAsNumpy(matrixVarName(elem, "_t"), elem->Mt()))
        .arg(matrixAsNumpy(matrixVarName(elem, "_s"), elem->Ms()));
}

QString roundTrip(const QList<Element*>& elems)
{
    QString resultT;
    QString resultS;
    for (int i = 0; i < elems.size(); i++)
    {
        Element *elem = elems[i];
        resultT += matrixVarName(elem, "_t") % " * ";
        resultS += matrixVarName(elem, "_s") % " * ";
    }
    resultT.truncate(resultT.length()-2); // remove last "* "
    resultS.truncate(resultS.length()-2); // remove last "* "
    return QStringLiteral("<p><code>M0_t = %1</code><br><code>M0_s = %2</code></p>").arg(resultT, resultS);
}

} // namespace Py

QString linkViewMatrix(Element *elem)
{
    return QStringLiteral("<a href='func://viewmatrix?elem=%1' class=elem_link>%2</a>")
            .arg(elem->id()).arg(elem->displayLabel());
}

QString roundTrip(const QList<Element*>& elems, bool hyperlinks)
{
    QString result = "";
    for (int i = 0; i < elems.size(); i++)
    {
        Element *elem = elems[i];
        result += (hyperlinks? linkViewMatrix(elem): elem->displayLabel())
                % ' ' % Z::Strs::multDot() % ' ';
    }
    result.truncate(result.length()-2); // remove last "* "
    return result;
}

QString elementTitleAndMatrices(Element *elem)
{
    QString result;
    QTextStream report(&result);

    report << QStringLiteral("<span class=elem_label>")
           << elem->displayLabel()
           << QStringLiteral("</span>");

    if (!elem->title().isEmpty())
        report << QStringLiteral(" <span class=elem_title>(")
               << elem->title()
               << QStringLiteral(")</span>");

    if (elem->hasOption(Element_Asymmetrical))
        report << qApp->translate("Z::Format", "<p>Forward-propagation matrices:");

    report << matrices(elem->Mt(), elem->Ms());

    if (elem->hasOption(Element_Asymmetrical))
        report << qApp->translate("Z::Format", "<p>Back-propagation matrices:")
               << matrices(elem->Mt_inv(), elem->Ms_inv());

    return result;
}

//------------------------------------------------------------------------------
//                                  FormatParam
//------------------------------------------------------------------------------

QString FormatParam::format(Z::Parameter* param)
{
    QStringList parts;

    // Base font is value font
    parts << QStringLiteral("<span style='") << html(ValueFont()) << QStringLiteral("'>");

    // Parameter label style
    parts << QStringLiteral("<span style='") << html(ParamLabelFont());
    if (!isElement)
        parts << QStringLiteral("; color:") << globalParamColorHtml();
    parts << QStringLiteral("'>");

    // Parameter label text
    parts << param->displayLabel()
          << QStringLiteral("</span>");

    _isReadOnly = false;

    if (includeDriver)
    {
        QString driverStr;

        if (isElement)
        {
            auto link = schema->paramLinks()->byTarget(param);
            _isReadOnly = link;
            if (link)
                driverStr = link->source()->displayLabel();
        }
        else
        {
            auto formula = schema->formulas()->get(param);
            _isReadOnly = formula;
            if (formula and not formula->deps().isEmpty())
                driverStr = formula->displayStr();
        }

        if (!driverStr.isEmpty())
        {
            // Driver style
            parts << QStringLiteral(" = <span style='");
            if (isElement)
            {
                // Element's param is driven by global param
                parts << html(ParamLabelFont())
                      << QStringLiteral("; color:") << globalParamColorHtml();
            }
            else
            {
                // Global param is driven by formula
                parts << html(FormulaFont());
            }
            parts << QStringLiteral("'>");

            // Driver text
            parts << driverStr
                  << QStringLiteral("</span>");
        }
    }

    if (includeValue)
    {
        parts << QStringLiteral(" = ");

        if (_isReadOnly)
            parts << QStringLiteral("<i>");

        // When HTML is available, it's better to format inverted units as negative degrees,
        // because digits in strings like "1/m" could be easily confused with the value itself, e.g. "1 1/m".
        auto unit = param->value().unit();
        if (unit == Z::Units::inv_m())
            parts << Z::format(param->value().value()) << QStringLiteral(" m<sup>&ndash;1</sup>");
        else if (unit == Z::Units::inv_m2())
            parts << Z::format(param->value().value()) << QStringLiteral(" m<sup>&ndash;2</sup>");
        else
            parts << param->value().displayStr();

        if (_isReadOnly)
            parts << QStringLiteral("</i>");
    }

    // weird, but the space after span is important!
    // without it vertical alignment calculated improperly sometimes
    parts << QStringLiteral("</span> ");

    return parts.join(QString());
}

//------------------------------------------------------------------------------
//                              FormatElemParams
//------------------------------------------------------------------------------

QString FormatElemParams::format(Element *elem)
{
    FormatParam f;
    f.schema = schema;
    f.includeValue = true;

    QStringList parts;
    for (Z::Parameter *param : elem->params())
    {
        if (!Z::Utils::defaultParamFilter()->check(param)) continue;

        parts << f.format(param).trimmed();
    }
    return parts.join(QStringLiteral(", "));
}

//------------------------------------------------------------------------------
//                              FormatPumpParams
//------------------------------------------------------------------------------

QString FormatPumpParams::format(PumpParams *pump)
{
    QStringList paramsInfo;
    for (Z::ParameterTS *param : *pump->params())
        paramsInfo << QStringLiteral(
            "<nobr><span style='%1'>%2</span><span style='%3'> = %4</span></nobr>")
            .arg(html(ParamLabelFont()), param->displayLabel(),
                 html(ValueFont()), param->value().displayStr());
    return paramsInfo.join(", ");
}

} // namespace Format
} // namespace Z

