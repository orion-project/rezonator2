#ifndef FORMULAEDITOR_H
#define FORMULAEDITOR_H

#include <QWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class ParamEditor;
class FormulaEditor;

// TODO move to more common location
template <class T> class OwnedPayload : public QObject
{
public:
    OwnedPayload(T* payload, QObject* parent) : QObject(parent), _payload(payload) {}
    ~OwnedPayload() { delete _payload; }
    T* payload() const { return _payload; }
private:
    T* _payload;
};

/**
    Extended parameter editor that can handle parameter and its formula.
*/
class ParamEditorEx : public QWidget
{
    Q_OBJECT

public:
    explicit ParamEditorEx(Z::Parameter* param, Z::Formulas* formulas, QWidget *parent = nullptr);

public slots:
    void apply();

private:
    Z::Parameter *_param, *_tmpParam;
    Z::Formula *_formula = nullptr;
    Z::Formula *_tmpFormula = nullptr;
    Z::Formulas *_formulas;
    QAction *_actnAddFormula, *_actnRemoveFormula;
    ParamEditor *_paramEditor;
    FormulaEditor *_formulaEditor = nullptr;
    bool _hasFormula = false;

    void populate();
    void addFormula();
    void removeFormula();
    void createFormulaEditor();
    void toggleFormulaView();
    void calculateFormula();
};

#endif // FORMULAEDITOR_H
