#include "FormatInfo.h"

#include "../core/Element.h"
#include "../core/Format.h"
#include "../core/Schema.h"
#include "../widgets/Appearance.h"

#include <QApplication>
#include <QFont>

using namespace Z::Gui;

namespace Z {
namespace Format {

QString matrix(const Z::Matrix& m)
{
    const char *matrixFmtString =
        "<table border=1 cellpadding=5 cellspacing=0>"
            "<tr><td align=right>%1</td><td align=right>%2</td></tr>"
            "<tr><td align=right>%3</td><td align=right>%4</td></tr>"
        "</table>";
    return QString::fromLatin1(matrixFmtString).arg(
        Z::format(m.A), Z::format(m.B), Z::format(m.C), Z::format(m.D));
}

QString matrices(const Z::Matrix& mt, const Z::Matrix& ms)
{
    static const char *matricesFmtString =
        "<table cellpadding=6 valign=middle>"
            "<tr>"
                "<td><b>M<sub>T</sub>&nbsp;&nbsp;=</b></td>"
                "<td>%1</td>"
                "<td><b>M<sub>S</sub>&nbsp;&nbsp;=</b></td>"
                "<td>%2</td>"
                "<td>"
                    "<p>&Delta;<sub>T</sub> = %3"
                    "<p>&Delta;<sub>S</sub> = %4"
                "</td>"
            "</tr>"
        "</table>";
    return QString::fromLatin1(matricesFmtString)
            .arg(matrix(mt))
            .arg(matrix(ms))
            .arg(mt.det())
            .arg(ms.det());
}

QString linkViewMatrix(Element *elem)
{
    return QStringLiteral("<a href='func://viewmatrix?elem=%1'>%2</a>")
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

QString elementTitle(Element *elem)
{
    QString disabled;
    if (elem->disabled()) disabled = qApp->translate("Z::Format", "(disabled)");
    return QStringLiteral("<span style='color: %1'>%2%3</span>").arg(
        elem->disabled()? "gray": "navy", elem->displayLabelTitle(), disabled);
}

QString elementTitleAndMatrices(Element *elem)
{
    QString result;
    if (elem->disabled())
        result += QStringLiteral("<div style='color: gray'>");
    result += elementTitle(elem) + matrices(elem->Mt(), elem->Ms());
    if (elem->hasOption(Element_Asymmetrical))
        result += QStringLiteral("<p>") + qApp->translate("Z::Format", "Back-propagation matrices:") +
                matrices(elem->Mt_inv(), elem->Ms_inv());
    if (elem->disabled())
        result += QStringLiteral("</div>");
    return result;
}

QString fontToHtmlStyles(const QFont& font)
{
    QStringList styles;
    if (font.bold())
        styles << QStringLiteral("bold");
    else if (font.italic())
        styles << QStringLiteral("italic");
    else
        styles << QStringLiteral("normal");
    styles << QString::number(font.pointSize()) % QStringLiteral("pt");
    styles << font.family();
    return QStringLiteral("font:") + styles.join(' ');
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
    parts << QStringLiteral("<span style='") << html(ParamLabelFont().small(smallName));
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
                parts << html(ParamLabelFont().small(smallName))
                      << QStringLiteral("; color:") << globalParamColorHtml();
            }
            else
            {
                // Global param is driven by formula
                parts << html(FormulaFont().small(smallName));
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

        parts << param->value().displayStr();

        if (_isReadOnly)
            parts << QStringLiteral("</i>");
    }

    parts << QStringLiteral("</span>");
    return parts.join('\n');
}

//------------------------------------------------------------------------------
//                              FormatElemParams
//------------------------------------------------------------------------------

QString FormatElemParams::format(Element *elem)
{
    FormatParam f;
    f.schema = schema;
    f.smallName = true;
    f.includeValue = true;

    QStringList parts;
    for (Z::Parameter *param : elem->params())
    {
        if (!Z::Utils::defaultParamFilter()->check(param)) continue;

        parts << f.format(param);
    }
    return parts.join(QStringLiteral(", "));
}

//------------------------------------------------------------------------------
//                              FormatPumpParams
//------------------------------------------------------------------------------

QString FormatPumpParams::format(Z::PumpParams *pump)
{
    QStringList paramsInfo;
    for (Z::ParameterTS *param : *pump->params())
        paramsInfo << QStringLiteral(
            "<nobr><span style='%1'>%2</span><span style='%3'> = %4</span></nobr>")
            .arg(html(ParamLabelFont().small()), param->displayLabel(),
                 html(ValueFont()), param->value().displayStr());
    return paramsInfo.join(", ");
}

} // namespace Format
} // namespace Z

