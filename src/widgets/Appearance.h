#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QFont>
#include <QColor>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
QT_END_NAMESPACE

namespace Z {
namespace Gui {

enum FontSize { FontSize_Normal, FontSize_Small };

void adjustSymbolFont(QFont& f, FontSize size = FontSize_Normal);
void adjustValueFont(QFont& f);

QFont codeEditorFont();
QFont formulaFont(FontSize size = FontSize_Normal);

QFont getElemLabelFont(FontSize size = FontSize_Normal);
QFont getParamLabelFont(FontSize size = FontSize_Normal);
QFont getValueFont();

template <class T> void setSymbolFont(T* target, FontSize size = FontSize_Normal)
{
    QFont f = target->font();
    adjustSymbolFont(f, size);
    target->setFont(f);
}

template <class T> void setValueFont(T* target)
{
    QFont f = target->font();
    adjustValueFont(f);
    target->setFont(f);
}

template <class T> void setFontStyle(T* target,
    bool bold, bool italic = false, bool underline = false, bool strikeout = false)
{
    QFont f = target->font();
    f.setBold(bold);
    f.setItalic(italic);
    f.setUnderline(underline);
    f.setStrikeOut(strikeout);
    target->setFont(f);
}

QLabel* headerlabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 60; }

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }
inline QColor globalParamColor() { return QColor(globalParamColorHtml()); }

void setFocusedBackground(QWidget *w, bool focused);

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
