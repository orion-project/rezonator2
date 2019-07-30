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

QString pumpParamsWithValues(Z::PumpParams *pump);

QString paramLabelStyle(bool isSmall = false);
QString formulaStyle(bool isSmall = false);
QString valueStyle();

struct FormatParam
{
    Schema* schema;
    bool includeValue = false;
    bool includeDriver = true;
    bool smallName = false;
    bool isElement = true;

    /// Parameter is read-only when it has a driver providing its value.
    /// So this property is only assigned when `includeDriver` requested.
    bool isReadOnly() const { return _isReadOnly; }

    QString format(Z::Parameter* param);

private:
    bool _isReadOnly = false;
};

struct FormatElemParams
{
    Schema* schema;

    QString format(Element *elem);
};

struct FormatPumpParams
{
    QString format(Z::PumpParams *pump);
};

} // namespace Format
} // namespace Z

#endif // FORMAT_INFO_H
