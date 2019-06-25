#ifndef FORMAT_INFO_H
#define FORMAT_INFO_H

#include "../core/Parameters.h"

class Schema;
class Element;

namespace Z {

class Matrix;
class PumpParams;

namespace Format {

QString elementTitle(Element *elem);
QString elementTitleAndMatrices(Element *elem);

QString matrix(const Z::Matrix& m);
QString matrices(const Z::Matrix& mt, const Z::Matrix& ms);

QString roundTrip(const QList<Element *> &elems, bool hyperlinks = false);

QString linkViewMatrix(Element *elem);

QString elemParamsHtml(Schema *schema, Element *elem, bool showLinksToGlobals = true);
QString pumpParamsHtml(Z::PumpParams *pump);

QString nameStyle();
QString valueStyle();

template <class TParam>
QString paramHtml(TParam *param)
{
    return QStringLiteral(
        "<nobr><span style='%1'>%2</span><span style='%3'> = %4</span></nobr>")
        .arg(nameStyle(), param->displayLabel(), valueStyle(), param->value().displayStr());
}

QString paramLabelHtml(Z::Parameter *param);
QString customParamLabelWithFormulaHtml(Z::Parameter *param, Schema* schema);

} // namespace Format
} // namespace Z

#endif // FORMAT_INFO_H
