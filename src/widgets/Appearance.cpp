#include "Appearance.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

void adjustValueFont(QFont& f)
{
#if defined(Q_OS_MAC)
    f.setPointSize(14);
#else
    if (f.pointSize() < 10)
        f.setPointSize(10);
#endif
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

void adjustCodeEditorFont(QFont &f)
{
#if defined(Q_OS_WIN)
    f.setFamily("Courier New");
    f.setPointSize(10);
#elif defined(Q_OS_MAC)
    f.setFamily("Monaco");
    f.setPointSize(13);
#else
    f.setFamily("monospace");
    f.setPointSize(11);
#endif
}


QLabel* symbolLabel(const QString& text)
{
    auto label = new QLabel(text);
    setSymbolFont(label);
    return label;
}

} // namespace Gui
} // namespace Z
