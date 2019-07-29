#include "Appearance.h"

#include "helpers/OriTools.h"

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

void adjustSymbolFont(QFont& f, FontSize size)
{
    f.setBold(true);
#if defined(Q_OS_WIN)
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setFamily(QStringLiteral("serif"));
#endif
    int sizePt =
#if defined(Q_OS_WIN)
        size == FontSize_Small ? 11 : 13;
#elif defined(Q_OS_MAC)
        size == FontSize_Small ? 16 : 20;
#else
        size == FontSize_Small ? 10 : 13;
#endif
    f.setPointSize(sizePt);
}

QFont codeEditorFont()
{
    QFont f = QApplication::font();
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
    return f;
}

QFont getElemLabelFont(FontSize size)
{
    QFont f = QApplication::font();
    adjustSymbolFont(f, size);
    return f;
}

QFont getParamLabelFont(FontSize size)
{
    QFont f = QApplication::font();
    adjustSymbolFont(f, size);
    return f;
}

QFont getValueFont()
{
    QFont f = QApplication::font();
    adjustValueFont(f);
    return f;
}

QFont formulaFont(FontSize size)
{
    auto f = getParamLabelFont(size);
    f.setItalic(true);
#if defined(Q_OS_WIN)
    f.setFamily(QStringLiteral("Times New Roman"));
#elif defined(Q_OS_MAC)
    f.setFamily(QStringLiteral("Times New Roman"));
#else
    f.setFamily(QStringLiteral("serif"));
#endif
    int sizePt =
#if defined(Q_OS_WIN)
        size == FontSize_Small ? 11 : 13;
#elif defined(Q_OS_MAC)
        size == FontSize_Small ? 16 : 20;
#else
        size == FontSize_Small ? 10 : 13;
#endif
    f.setPointSize(sizePt);
    return f;
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
