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
    ElemLabelFont& small(bool on = true) { _small = on; return *this; }
    int key() const { return _small; }
    QFont get() const;
private:
    bool _small = false;
};

using ParamLabelFont = ElemLabelFont;
using PumpLabelFont = ElemLabelFont;

struct FormulaFont
{
    FormulaFont& small(bool on = true) { _small = on; return *this; }
    int key() const { return _small; }
    QFont get() const;
private:
    bool _small = false;
};

QLabel* headerlabel(const QString& text);

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 60; }

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }

void setFocusedBackground(QWidget *w, bool focused);

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
