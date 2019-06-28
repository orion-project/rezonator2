#ifndef FORMAT_INFO_H
#define FORMAT_INFO_H

#include "../core/Parameters.h"

class Schema;
class Element;

namespace Z {

class Matrix;
class PumpParams;

namespace Format {

// TODO: Format functions should be unified.
// Now, part of them supposes target control already have the proper font set up (face and size),
// while another part includes font specification into result HTML (`nameStyle()` and `valueStyle()`).


QString elementTitle(Element *elem);
QString elementTitleAndMatrices(Element *elem);

QString matrix(const Z::Matrix& m);
QString matrices(const Z::Matrix& mt, const Z::Matrix& ms);

QString roundTrip(const QList<Element *> &elems, bool hyperlinks = false);

QString linkViewMatrix(Element *elem);

QString elemParamLabel(Z::Parameter* param, Schema *schema, bool showLinksToGlobals = true);
QString elemParamLabelAndValue(Z::Parameter* param, Schema *schema, bool showLinksToGlobals = true);
QString elemParamsWithValues(Element *elem, Schema *schema, bool showLinksToGlobals = true);
QString pumpParamsWithValues(Z::PumpParams *pump);

QString nameStyle(bool isSmall = false);
QString formulaStyle(bool isSmall = false);
QString valueStyle();

template <class TParam>
QString paramLabelAndValue(TParam *param)
{
    return QStringLiteral(
        "<nobr><span style='%1'>%2</span><span style='%3'> = %4</span></nobr>")
        .arg(nameStyle(), param->displayLabel(), valueStyle(), param->value().displayStr());
}

QString paramLabel(Z::Parameter *param);
QString customParamLabel(Z::Parameter *param, Schema* schema, bool showFormula = true);

struct FormatParam
{
    Schema* schema;
    bool includeValue = false;
    bool includeDriver = true;
    bool smallName = false;
    bool isElement = true;
    bool isReadOnly = false;

    QString format(Z::Parameter* param);
};

} // namespace Format
} // namespace Z

#endif // FORMAT_INFO_H
