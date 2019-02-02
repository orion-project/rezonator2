#ifndef Z_IO_CLIPBOARD_H
#define Z_IO_CLIPBOARD_H

#include <QList>

class Element;

namespace Z {
namespace IO {
namespace Clipboard {

void setElements(const QList<Element*>& elements);
QList<Element*> getElements();

} // namespace Clipboard
} // namespace IO
} // namespace Z

#endif // Z_IO_CLIPBOARD_H
