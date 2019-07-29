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

struct ValueFont
{
    ValueFont& bold() { _bold = true; return *this; }
    ValueFont& readOnly(bool on = true) { _readOnly = on; return *this; }
    QFont get() const;
private:
    bool _bold = false;
    bool _readOnly = false;
};

struct CodeEditorFont
{
    QFont get() const;
};

struct ElemLabelFont
{
    ElemLabelFont& small() { _small = true; return *this; }
    QFont get() const;
    QString html() const;
private:
    bool _small = false;
};

struct ParamLabelFont
{
    ParamLabelFont& small() { _small = true; return *this; }
    QFont get() const;
private:
    bool _small = false;
};

struct FormulaFont
{
    FormulaFont& small() { _small = true; return *this; }
    QFont get() const;
private:
    bool _small = false;
};

template <class T> void setSymbolFont(T* target, FontSize size = FontSize_Normal)
{
    QFont f = target->font();
    adjustSymbolFont(f, size);
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
