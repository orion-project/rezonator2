#include "Appearance.h"

#include "helpers/OriTools.h"

#include <QApplication>
#include <QLabel>
#include <QStyle>

namespace Z {
namespace Gui {

QString reportStyleSheet()
{
    static auto styleSheet = QString::fromLatin1(
        ".value {%1} "
        ".param {%2}"
        ".elem_label {%3}"
        ".elem_link {text-decoration:none}"
    ).arg(
        html(ValueFont()),
        html(ParamLabelFont()),
        html(ElemLabelFont())
    );
    return styleSheet;
}

QString fontToHtmlStyles(const QFont& font)
{
    QStringList styles;
    styles << QStringLiteral("font:");
    if (font.bold())
        styles << QStringLiteral("bold");
    else if (font.italic())
        styles << QStringLiteral("italic");
    else
        styles << QStringLiteral("normal");
    styles << QChar(' ');
    styles << QString::number(font.pointSize()) % QStringLiteral("pt");
    styles << QChar(' ');
    styles << QChar('"') << font.family() << QChar('"');
    return styles.join(QString());
}

QFont ValueFont::get() const
{
    QFont f = QApplication::font();
    if (_bold)
        f.setBold(true);
    if (_readOnly)
        f.setItalic(true);
#if defined(Q_OS_MAC)
    f.setPointSize(14);
#else
    if (f.pointSize() < 10)
        f.setPointSize(10);
#endif
    return f;
}

QFont CodeEditorFont::get() const
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

QFont ElemLabelFont::get() const
{
    QFont f = QApplication::font();
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
        11
#elif defined(Q_OS_MAC)
        16
#else
        10
#endif
    ;
    f.setPointSize(sizePt);
    return f;
}

QFont FormulaFont::get() const
{
    auto f = QApplication::font();
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
        11
#elif defined(Q_OS_MAC)
        16
#else
        10
#endif
    ;
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
    label->setStyleSheet("color:SteelBlue");
    return label;
}

void setFocusedBackground(QWidget *w, bool focused)
{
    QPalette p;
    if (focused)
        p.setColor(QPalette::Background, Ori::Color::blend(p.color(QPalette::Button), p.color(QPalette::Highlight), 0.2));
    w->setAutoFillBackground(focused);
    w->setPalette(p);
}

} // namespace Gui
} // namespace Z
