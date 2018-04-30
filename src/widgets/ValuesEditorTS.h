#ifndef VALUESEDITORTS_H
#define VALUESEDITORTS_H

#include "../core/Values.h"
#include "../core/CommonTypes.h"

#include <QMap>
#include <QWidget>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QLabel;
class QToolButton;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}

class UnitComboBox;

class ValueEditorTS : public QWidget
{
    Q_OBJECT
public:
    explicit ValueEditorTS(const QString& label, const QString& symbol, const Z::ValueTS& value);
    void setValue(const Z::ValueTS& value);
    Z::ValueTS value() const;
    void setFocus(Z::WorkPlane plane);
public slots:
    void swapValues();
    void assignTtoS();
    void assignStoT();
signals:
    void goingFocusNext(Z::WorkPlane plane);
    void goingFocusPrev(Z::WorkPlane plane);
private:
    Ori::Widgets::ValueEdit *_editorT, *_editorS;
    UnitComboBox *_unitsSelector;
    QLabel *_label, *_symbol;
private slots:
    void editorFocused(bool focus);
    void editorKeyPressed(int key);
    friend class ValuesEditorTS;
};

class ValuesEditorTS : public QVBoxLayout
{
    Q_OBJECT
public:
    ValuesEditorTS();
    ValuesEditorTS(const QVector<ValueEditorTS*>& editors);
    ValueEditorTS* firstEditor() { return _editors.first(); }
    void adjustSymbolsWidth();
    void addEditor(ValueEditorTS *editor);
public slots:
    void swapValues();
    void assignTtoS();
    void assignStoT();
private:
    QVector<ValueEditorTS*> _editors;
    QLabel *_headerT, *_headerS, *_unitSpacer;
    void goingFocusNext(Z::WorkPlane plane);
    void goingFocusPrev(Z::WorkPlane plane);
};

#endif // VALUESEDITORTS_H
