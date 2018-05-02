#include "Appearance.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

QFont getSymbolFontSm()
{
    QFont f = QApplication::font();
    adjustSymbolFontSm(f);
    return f;
}

QFont getValueFont()
{
    QFont f = QApplication::font();
    adjustValueFont(f);
    return f;
}

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
#if defined(Q_OS_WIN)
    f.setPointSize(13);
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setPointSize(20);
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setPointSize(13);
    f.setFamily(QStringLiteral("serif"));
#endif
}

void adjustSymbolFontSm(QFont& f)
{
    f.setBold(true);
#if defined(Q_OS_WIN)
    f.setPointSize(13);
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setPointSize(16);
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setPointSize(13);
    f.setFamily(QStringLiteral("serif"));
#endif
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

QString fontToHtmlStyles(const QFont& font)
{
    QStringList styles;
    if (font.bold())
        styles << QStringLiteral("bold");
    else if (font.italic())
        styles << QStringLiteral("italic");
    else
        styles << QStringLiteral("normal");
    styles << QString::number(font.pointSize()) % QStringLiteral("pt");
    styles << font.family();
    return QStringLiteral("font:") + styles.join(' ');
}

} // namespace Gui
} // namespace Z
