#ifndef PARAM_EDITOR_EX_H
#define PARAM_EDITOR_EX_H

#include <QWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class ParamEditor;
class FormulaEditor;

/**
    Extended parameter editor that can handle parameter and its formula.
*/
class ParamEditorEx : public QWidget
{
    Q_OBJECT

public:
    explicit ParamEditorEx(Z::Parameter* param, Z::Formulas* formulas, QWidget *parent = nullptr);
    ~ParamEditorEx();

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
    void unitChanged(Z::Unit unit);
};

#endif // PARAM_EDITOR_EX_H
