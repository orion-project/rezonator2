#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QColor>
#include <QFont>
#include <QMap>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
class QTabWidget;
class QTextBrowser;
class QToolBar;
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

QLabel* makeHeaderLabel(const QString& text);
QTabWidget* makeBorderlessTabs();
QToolBar* makeToolbar(std::initializer_list<QObject*> items, const QString& helpTopic = QString());

/// Preferred fixed width of unit selector combo boxes
inline int unitsSelectorWidth() { return 63; }

inline QString globalParamColorHtml() { return QStringLiteral("#000080"); }

// These are theme colors, have to be in sync with app.qss
// TODO: Need a way to extract them from there automatically
inline QColor baseColor() { return QColor(0xdadbde); }
inline QColor selectionColor() { return QColor("steelBlue"); }
inline QColor lightSelectionColor() { return QColor(0x9db4c5); }
inline QColor textColor() { return QColor(0x222222); }
inline QColor mutedTextColor() { return QColor(0x777788); }
inline QColor paperColor() { return QColor(0xffffff); }
inline QColor midPaperColor() { return QColor(0xf6f6f8); }
inline QColor darkPaperColor() { return QColor(0xeaebee); }
inline QColor yellowPaperColor() { return QColor(0xffffdc); }

void setFocusedBackground(QWidget *w, bool focused);

enum ItemDataRole { DisabledRole = Qt::UserRole+100 };

void applyTextBrowserStyleSheet(QTextBrowser* browser, const QString& cssResourcePath);
void editAppStyleSheet();

QSize toolbarIconSize();

void applyStylesheet(QWidget*);

} // namespace Gui
} // namespace Z

#endif // APPEARANCE_H
