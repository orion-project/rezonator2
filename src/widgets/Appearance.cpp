#include "Appearance.h"

#include "helpers/OriTools.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

QFont getSymbolFont()
{
    QFont f = QApplication::font();
    adjustSymbolFont(f);
    return f;
}

QFont getSymbolFontSm()
{
    QFont f = QApplication::font();
    adjustSymbolFontSm(f);
    return f;
}

QFont getFormulaFont()
{
    QFont f = QApplication::font();
    adjustSymbolFont(f);
    f.setBold(false);
    f.setItalic(true);
    return f;
}

QFont getFormulaFontSm()
{
    QFont f = QApplication::font();
    adjustSymbolFontSm(f);
    f.setBold(false);
    f.setItalic(true);
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
    f.setPointSize(11);
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setPointSize(16);
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setPointSize(10);
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

QLabel* headerlabel(const QString& text)
{
    auto label = new QLabel(text);
    auto f = label->font();
    f.setPointSize(f.pointSize() + 2);
    f.setBold(true);
    label->setFont(f);
    return label;
}

void setFocusedBackground(QWidget *w, bool focused)
{
    QPalette p;

    if (focused)
    {
        p.setColor(QPalette::Background, Ori::Color::blend(p.color(QPalette::Button), p.color(QPalette::Highlight), 0.2));
        w->setAutoFillBackground(true);
    }
    else
        w->setAutoFillBackground(false);

    w->setPalette(p);
}

} // namespace Gui
} // namespace Z
