#ifndef Z_IO_CLIPBOARD_H
#define Z_IO_CLIPBOARD_H

#include <QList>

class Element;
class Schema;

namespace Z {
namespace IO {
namespace Clipboard {

void copyElements(const QList<Element*>& elements);
void pasteElements(Schema* schema);

} // namespace Clipboard
} // namespace IO
} // namespace Z

#endif // Z_IO_CLIPBOARD_H
