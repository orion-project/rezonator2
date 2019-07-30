#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QFont>
#include <QMap>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
QT_END_NAMESPACE

namespace Z {
namespace Gui {

QString fontToHtmlStyles(const QFont& font);

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
    int key() const { return _small; }
    QFont get() const;
private:
    bool _small = false;
};

using ParamLabelFont = ElemLabelFont;
using PumpLabelFont = ElemLabelFont;

struct FormulaFont
{
    FormulaFont& small() { _small = true; return *this; }
    QFont get() const;
private:
    bool _small = false;
};

template <class T> QString html(const T& font)
{
    int key = font.key();
    static QMap<int, QString> htmls;
    if (!htmls.contains(key))
        htmls.insert(key, fontToHtmlStyles(font.get()));
    return htmls[key];
}

QLabel* headerlabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 60; }

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }

void setFocusedBackground(QWidget *w, bool focused);

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
