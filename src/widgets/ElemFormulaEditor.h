#ifndef ELEM_FORMULA_EDITOR_H
#define ELEM_FORMULA_EDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
class QTextEdit;
class QToolBar;
class QCheckBox;
QT_END_NAMESPACE

class ElemFormula;
class ParamsEditor;

namespace Ori::Widgets {
class CodeEditor;
}

class ElemFormulaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ElemFormulaEditor(ElemFormula* sourceElem, ElemFormula *workingCopy);

    ~ElemFormulaEditor() override;

    bool canCopy();
    bool canPaste();
    void selectAll();
    void copy();
    void paste();

    void populateWindowMenu(QMenu* menu);

    ElemFormula* sourceElem() { return _sourceElem; }
    ElemFormula* workingCopy() { return _workingCopy; }

    bool isChanged() const { return _isChanged; }
    void setIsChanged(bool on) { _isChanged = on; }

    // ElemFormulaWindow calls it before saving schema file
    void applyWorkingValues();
    
    void resetModifyFlag();

signals:
    void onModify();
    void onApply();

private:
    ElemFormula* _sourceElem;
    ElemFormula* _workingCopy;
    ParamsEditor* _paramsEditor;
    QAction *_actnApplyChanges, *_actnResetChanges, *_actnCheckCode, *_actnClearLog, *_actnShowHelp,
        *_actnParamAdd, *_actnParamDelete, *_actnParamDescr, *_actnParamMoveUp, *_actnParamMoveDown;
    Ori::Widgets::CodeEditor *_codeEditor;
    QTextEdit *_logView;
    QToolBar *_toolbar;
    QMenu *_menuParam;
    QWidget *_stubNoParams;
    bool _isChanged = false;
    bool _lockEvents = false;

    void createActions();
    void createToolbar();
    void paramsChanged();
    void codeModified();
    void updateParamsEditorVisibility();
    void populate();
    void collect();
    void reset();
    void checkFormula();
    void clearLog();
    void showHelp();
    void createParameter();
    void deleteParameter();
    void annotateParameter();
    void moveParameterUp();
    void moveParameterDown();
};

#endif // ELEM_FORMULA_EDITOR_H
