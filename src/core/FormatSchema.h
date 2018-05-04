#ifndef Z_FORMAT_SCHEMA_H
#define Z_FORMAT_SCHEMA_H

#include <QString>

class Schema;
class Element;

namespace Z {

class PumpParams;

namespace Fmt {

QString elemParamsHtml(Schema *schema, Element *elem, bool showLinksToGlobals = true);
QString pumpParamsHtml(Z::PumpParams *pump);

template <class TParam>
QString paramHtml(TParam *param);

} // namespace Fmt
} // namespace Z

#endif // Z_FORMAT_SCHEMA_H
