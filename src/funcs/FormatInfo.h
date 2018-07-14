#ifndef FORMAT_INFO_H
#define FORMAT_INFO_H

#include <QString>

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

template <class TParam>
QString paramHtml(TParam *param);

} // namespace Format
} // namespace Z

#endif // FORMAT_INFO_H
