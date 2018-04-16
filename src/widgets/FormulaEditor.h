#ifndef FORMULA_EDITOR_H
#define FORMULA_EDITOR_H

#include <QTabWidget>

#include "../core/Formula.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class FormulaEditor : public QTabWidget
{
    Q_OBJECT

public:
    explicit FormulaEditor(Z::Formula *formula, QWidget *parent = nullptr);

    void setFocus();

private:
    Z::Formula *_formula;
    QTextEdit *_codeEditor;
    QLabel *_statusLabel;
    QTimer *_recalcTimer;

    void codeChanged();
    void showStatus();
    void calculate();
};

#endif // FORMULA_EDITOR_H
