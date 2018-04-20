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

void adjustSymbolFont(QFont& f);
void adjustValueFont(QFont& f);
void adjustCodeEditorFont(QFont& f);

template <class T> void setSymbolFont(T* target)
{
    QFont f = target->font();
    adjustSymbolFont(f);
    target->setFont(f);
}

template <class T> void setValueFont(T* target)
{
    QFont f = target->font();
    adjustValueFont(f);
    target->setFont(f);
}

template <class T> void setCodeEditorFont(T* target)
{
    QFont f = target->font();
    adjustCodeEditorFont(f);
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

QLabel* symbolLabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 60; }

QString fontToHtmlStyles(const QFont& font);

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }
inline QColor globalParamColor() { return QColor(globalParamColorHtml()); }

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
