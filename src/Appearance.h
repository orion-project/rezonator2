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

QString reportStyleSheet();
QString fontToHtmlStyles(const QFont& font);

inline int boolKey(bool on, int offset = 0)
{
    return (on ? 1 : 0) << offset;
}

struct ValueFont
{
    ValueFont& bold(bool on = true) { _bold = on; return *this; }
    ValueFont& readOnly(bool on = true) { _readOnly = on; return *this; }
    int key() const { return boolKey(_bold) | boolKey(_readOnly, 1); }
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
    int key() const { return 0; }
    QFont get() const;
};

using ParamLabelFont = ElemLabelFont;
using PumpLabelFont = ElemLabelFont;

struct FormulaFont
{
    int key() const { return 0; }
    QFont get() const;
};

template <class FontStruct> QString html(const FontStruct& font)
{
    int key = font.key();
    static QMap<int, QString> htmls;
    if (!htmls.contains(key))
        htmls.insert(key, fontToHtmlStyles(font.get()));
    return htmls[key];
}

QLabel* headerlabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 63; }

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }

void setFocusedBackground(QWidget *w, bool focused);

enum ItemDataRole { DisabledRole = Qt::UserRole+100 };

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
