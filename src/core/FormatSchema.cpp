#include "FormatSchema.h"

#include "Schema.h"
#include "../widgets/Appearance.h"

namespace Z {
namespace Fmt {

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

} // namespace Fmt
} // namespace Z
