#ifndef FORMAT_INFO_H
#define FORMAT_INFO_H

#include "../core/Parameters.h"

class Schema;
class Element;
class PumpParams;

namespace Z {

class Matrix;

namespace Format {

QString elementTitleAndMatrices(Element *elem);

QString matrix(const Z::Matrix& m);
QString matrices(const Z::Matrix& mt, const Z::Matrix& ms);

QString roundTrip(const QList<Element *> &elems, bool hyperlinks = false);

QString linkViewMatrix(Element *elem);

struct FormatParam
{
    Schema* schema;
    bool includeValue = false;
    bool includeDriver = true;
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
    QString format(PumpParams *pump);
};

} // namespace Format
} // namespace Z

#endif // FORMAT_INFO_H
