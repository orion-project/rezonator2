#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QString>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
QT_END_NAMESPACE

namespace Z {
namespace Gui {

void setValueFont(QWidget*);
void setSymbolFont(QWidget*);

QLabel* symbolLabel(const QString& text);

inline int unitsSelectorWidth() { return 60; }

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
