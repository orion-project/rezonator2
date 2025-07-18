#ifndef FORMULA_EDITOR_H
#define FORMULA_EDITOR_H

#include <QTabWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class FormulaEditor : public QWidget
{
    Q_OBJECT

public:
    struct Options
    {
        /// Editing formula.
        Z::Formula *formula;

        /// Actual target parameter of formula.
        /// It can diferent from formula.target() because of formula can drive temporary param,
        /// but this actual param is required to check deps to globals for circular links.
        Z::Parameter *targetParam = nullptr;

        /// List of global parameter formula can depends on.
        Z::Parameters *globalParams = nullptr;

        /// List of all formulas.
        /// It is used to check circular dependencies when appending params as formula deps.
        Z::Formulas *formulas = nullptr;
    };

public:
    explicit FormulaEditor(Options opts, QWidget *parent = nullptr);

    void setFocus();
    void calculate();

private:
    Z::Formula *_formula;
    Z::Parameter *_targetParam;
    Z::Parameters *_globalParams;
    Z::Formulas *_formulas;
    QTextEdit *_codeEditor;
    QLabel *_statusLabel;
    QTimer *_recalcTimer;
};

#endif // FORMULA_EDITOR_H
