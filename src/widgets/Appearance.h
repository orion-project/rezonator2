#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QFont>

QT_BEGIN_NAMESPACE
class QFont;
class QLabel;
class QWidget;
QT_END_NAMESPACE

namespace Z {
namespace Gui {

void adjustSymbolFont(QFont& f);
void adjustValueFont(QFont& f);

template <class T> void setSymbolFont(T* target) {
    QFont f = target->font();
    adjustSymbolFont(f);
    target->setFont(f);
}

template <class T> void setValueFont(T* target) {
    QFont f = target->font();
    adjustValueFont(f);
    target->setFont(f);
}

QLabel* symbolLabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 60; }

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
