#ifndef FORMULA_EDITOR_H
#define FORMULA_EDITOR_H

#include <QTabWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class ParamsListWidget;

class FormulaEditor : public QTabWidget
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
    };

public:
    explicit FormulaEditor(Options opts, QWidget *parent = nullptr);

    void setFocus();
    void calculate();

private:
    Z::Formula *_formula;
    Z::Parameter *_targetParam;
    Z::Parameters *_globalParams;
    QTextEdit *_codeEditor;
    QLabel *_statusLabel;
    QTimer *_recalcTimer;
    ParamsListWidget *_paramsList;
    int _tabIndexCode, _tabIndexParams;

    QWidget* makeEditorTab();
    QWidget* makeParamsTab();

    void addParam();
    void removeParam();
    void showParamsCount();
};

#endif // FORMULA_EDITOR_H
