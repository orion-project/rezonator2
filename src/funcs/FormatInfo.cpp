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
    const char *matricesFmtString =
        "<table cellpadding=6 valign=middle><tr>"
            "<td><b>M<sub>T</sub>&nbsp;&nbsp;=</b></td>"
            "<td>%1</td>"
            "<td>&nbsp;</td>"
            "<td><b>M<sub>S</sub>&nbsp;&nbsp;=</b></td>"
            "<td>%2</td>"
        "</tr></table>";
    return  QString::fromLatin1(matricesFmtString).arg(matrix(mt), matrix(ms));
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
        result += "<div style='color: gray'>";
    result += elementTitle(elem) + matrices(elem->Mt(), elem->Ms());
    if (elem->disabled())
        result += "</div>";
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

QString elemParamsHtml(Schema *schema, Element *elem, bool showLinksToGlobals)
{
    QStringList paramsInfo;
    for (Z::Parameter *param : elem->params())
    {
        if (!Z::Utils::defaultParamFilter()->check(param)) continue;

        if (showLinksToGlobals)
        {
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
            paramsInfo << QStringLiteral("<span style='%1'>%2</span> = %3")
                            .arg(nameStyle(), param->displayLabel(), valueStr);
        }
        else
            paramsInfo << paramHtml(param);
    }
    return paramsInfo.join(", ");
}

QString pumpParamsHtml(Z::PumpParams *pump)
{
    QStringList paramsInfo;
    for (Z::ParameterTS *param : *pump->params())
        paramsInfo << paramHtml(param);
    return paramsInfo.join(", ");
}

template <class TParam>
QString paramHtml(TParam *param)
{
    return QStringLiteral(
        "<nobr><span style='%1'>%2</span><span style='%3'> = %4</span></nobr>")
        .arg(nameStyle(), param->displayLabel(), valueStyle(), param->value().displayStr());
}
template QString paramHtml(Z::Parameter *param);
template QString paramHtml(Z::ParameterTS *param);

} // namespace Format
} // namespace Z

