#ifndef FORMATINFO_H
#define FORMATINFO_H

#include <QString>
#include <QApplication>

#include "../core/Element.h"

namespace Z {
namespace Format {

QString elementTitle(Element *elem);
QString elementTitleAndMatrices(Element *elem);
QString matrix(const Z::Matrix& m);
QString matrices(const Z::Matrix& mt, const Z::Matrix& ms);
QString roundTrip(const Elements &elems, bool hyperlinks = false);
QString linkViewMatrix(Element *elem);

} // namespace Format
} // namespace Z

#endif // FORMATINFO_H
