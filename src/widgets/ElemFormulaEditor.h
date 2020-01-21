#ifndef ELEM_FORMULA_EDITOR_H
#define ELEM_FORMULA_EDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QToolBar;
QT_END_NAMESPACE

class ElemFormula;

class ElemFormulaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ElemFormulaEditor(ElemFormula* elem, QWidget *parent = nullptr);

    static bool editFormula(ElemFormula* elem);

private slots:
    void checkFormula();
    void clearLog();
    void showHelp();

private:
    ElemFormula* _elem;
    QAction *_actnCheck, *_actnClearLog, *_actnHelp;
    QTextEdit *_codeEditor, *_logView;
    QToolBar *_toolbar;

    void createActions();
    void createToolbar();
};

#endif // ELEM_FORMULA_EDITOR_H
