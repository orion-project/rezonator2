#include "Appearance.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

void adjustValueFont(QFont& f)
{
    if (f.pointSize() < 10)
        f.setPointSize(10);
}

void adjustSymbolFont(QFont& f)
{
    f.setBold(true);
#ifdef Q_OS_MAC
    // Font looks too small compared to those on Ubuntu (Unity, xfce) or Windows.
    // At least on Macbook Air, can't check other devices. Make it a bit bigger.
    f.setPointSize(16);
#else
    f.setPointSize(13);
#endif
    f.setFamily("Times New Roman");
}

QLabel* symbolLabel(const QString& text)
{
    auto label = new QLabel(text);
    setSymbolFont(label);
    return label;
}

} // namespace Gui
} // namespace Z
