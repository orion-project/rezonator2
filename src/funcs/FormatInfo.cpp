#include "FormatInfo.h"

#include "../core/Element.h"
#include "../core/Format.h"
#include "../core/Schema.h"
#include "../widgets/Appearance.h"

#include <QApplication>
#include <QFont>

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

QString nameStyle()
{
    static QString style(fontToHtmlStyles(Z::Gui::getSymbolFontSm()));
    return style;
}

QString valueStyle()
{
    static QString style(fontToHtmlStyles(Z::Gui::getValueFont()));
    return style;
}

QString elemParamLabel(Schema *schema, Z::Parameter* param, bool showLinksToGlobals)
{
    auto labelStr = QStringLiteral("<span style='%1'>%2</span>")
                    .arg(nameStyle(), param->displayLabel());
    if (!showLinksToGlobals)
        return labelStr;

    auto link = schema->paramLinks()->byTarget(param);
    if (!link)
        return labelStr;

    return QStringLiteral("%1 = <span style='%2; color:%3'>%4</span>")
                .arg(labelStr,
                     nameStyle(),
                     Z::Gui::globalParamColorHtml(),
                     link->source()->displayLabel());
}

QString elemParamLabelAndValue(Schema *schema, Z::Parameter* param, bool showLinksToGlobals)
{
    if (not showLinksToGlobals)
        return paramLabelAndValue(param);

    QString valueStr;
    auto link = schema->paramLinks()->byTarget(param);
    if (link)
        valueStr = QStringLiteral("<span style='%1; color:%2'>%3</span> = <span style='%4'><i>%5</i></span>")
                    .arg(nameStyle(),
                         Z::Gui::globalParamColorHtml(),
                         link->source()->displayLabel(),
                         valueStyle(),
                         param->value().displayStr());
    else
        valueStr = QStringLiteral("<span style='%1'>%2</span>")
                    .arg(valueStyle(), param->value().displayStr());
    return QStringLiteral("<span style='%1'>%2</span> = %3")
                    .arg(nameStyle(), param->displayLabel(), valueStr);
}

QString elemParamsWithValues(Schema *schema, Element *elem, bool showLinksToGlobals)
{
    QStringList paramsInfo;
    for (Z::Parameter *param : elem->params())
    {
        if (!Z::Utils::defaultParamFilter()->check(param)) continue;

        paramsInfo << elemParamLabelAndValue(schema, param, showLinksToGlobals);
    }
    return paramsInfo.join(", ");
}

QString pumpParamsWithValues(Z::PumpParams *pump)
{
    QStringList paramsInfo;
    for (Z::ParameterTS *param : *pump->params())
        paramsInfo << paramLabelAndValue(param);
    return paramsInfo.join(", ");
}

QString paramLabel(Z::Parameter *param)
{
    return QStringLiteral("<b>%2</b>").arg(param->displayLabel());
}

QString customParamLabel(Z::Parameter *param, Schema* schema, bool showFormula)
{
    QString formulaDescr;
    if (showFormula)
    {
        auto formula = schema->formulas()->get(param);
        if (formula and not formula->deps().isEmpty())
            formulaDescr = QStringLiteral(" <span style='font-weight:normal; font-style:italic'>= %1</span>").arg(formula->displayStr());
    }
    return QStringLiteral("<span style='color:%1; font-weight:bold'>%2</span>%3")
        .arg(Z::Gui::globalParamColorHtml(), param->label(), formulaDescr);
}

} // namespace Format
} // namespace Z

