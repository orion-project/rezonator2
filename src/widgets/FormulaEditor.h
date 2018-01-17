#ifndef FORMULAEDITOR_H
#define FORMULAEDITOR_H

#include <QWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QTimer;
class QTextEdit;
QT_END_NAMESPACE

class ParamEditor;

class FormulaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FormulaEditor(Z::Parameter* param, Z::Formulas* formulas, QWidget *parent = nullptr);
    ~FormulaEditor();

public slots:
    void apply();

private:
    Z::Parameter *_param, *_tmpParam;
    Z::Formula *_formula = nullptr;
    Z::Formula *_tmpFormula = nullptr;
    Z::Formulas *_formulas;
    QAction *_actnAddFormula, *_actnRemoveFormula;
    QTextEdit *_codeEditor = nullptr;
    QLabel *_formulaStatus = nullptr;
    QTimer *_recalcTimer = nullptr;
    ParamEditor *_paramEditor;
    bool _hasFormula = false;

    void populate();
    void addFormula();
    void removeFormula();
    void createCodeEditor();
    void toggleFormulaView();
    void formulaCodeChanged();
    void showFormulaStatus();
    void calculateFormula();
};

#endif // FORMULAEDITOR_H
