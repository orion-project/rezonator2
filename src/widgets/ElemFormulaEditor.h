#ifndef ELEM_FORMULA_EDITOR_H
#define ELEM_FORMULA_EDITOR_H

#include "../core/Parameters.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
class QTextEdit;
class QToolBar;
class QCheckBox;
QT_END_NAMESPACE

class ElemFormula;
class ParamsEditor;

class ElemFormulaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ElemFormulaEditor(ElemFormula* elem, bool fullToolbar);

    bool canCopy();
    bool canPaste();
    void selectAll();
    void copy();
    void paste();

    void populateWindowMenu(QMenu* menu);

private slots:
    void saveChanges();
    void resetChanges();
    void checkFormula();
    void clearLog();
    void showHelp();
    void createParameter();
    void deleteParameter();
    void annotateParameter();

private:
    ElemFormula* _element;
    ParamsEditor* _paramsEditor;
    Z::Parameters _parameters;
    QAction *_actnSaveChanges, *_actnResetChanges, *_actnCheckCode, *_actnClearLog, *_actnShowHelp,
        *_actnParamAdd, *_actnParamDelete, *_actnParamDescr;
    QTextEdit *_codeEditor, *_logView;
    QToolBar *_toolbar;
    QMenu *_menuParam;
    QCheckBox *_flagHasTandSMatrices;

    void createActions();
    void createToolbar(bool full);
};

#endif // ELEM_FORMULA_EDITOR_H
