#ifndef Z_IO_CLIPBOARD_H
#define Z_IO_CLIPBOARD_H

#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsView;
QT_END_NAMESPACE

class Element;
class PumpParams;

namespace Z {
namespace IO {
namespace Clipboard {

void setElements(const QList<Element*>& elements);
QList<Element*> getElements();

void setPumps(const QList<PumpParams*>& pumps);
QList<PumpParams*> getPumps();

void setImage(QGraphicsView* view, bool transparent = false);

} // namespace Clipboard
} // namespace IO
} // namespace Z

#endif // Z_IO_CLIPBOARD_H
