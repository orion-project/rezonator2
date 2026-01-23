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
    explicit ElemFormulaEditor(ElemFormula* sourceElem, bool fullToolbar);
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
    void populateValues();
    void applyValues();

signals:
    void onChanged();
    void onSaved();

private slots:
    void saveChanges();
    void resetChanges();
    void checkFormula();
    void clearLog();
    void showHelp();
    void createParameter();
    void deleteParameter();
    void annotateParameter();
    void moveParameterUp();
    void moveParameterDown();

private:
    /// This construct should be called only during schema loading
    /// when working copy of element is loaded from schema file
    /// instead of being created from the source element
    explicit ElemFormulaEditor(ElemFormula* sourceElem, ElemFormula *workingCopy, bool fullToolbar);

    ElemFormula* _sourceElem;
    ElemFormula* _workingCopy;
    ParamsEditor* _paramsEditor;
    QAction *_actnSaveChanges, *_actnResetChanges, *_actnCheckCode, *_actnClearLog, *_actnShowHelp,
        *_actnParamAdd, *_actnParamDelete, *_actnParamDescr, *_actnParamMoveUp, *_actnParamMoveDown;
    Ori::Widgets::CodeEditor *_codeEditor;
    QTextEdit *_logView;
    QToolBar *_toolbar;
    QMenu *_menuParam;
    QCheckBox *_flagHasMatricesTS;
    QWidget *_stubNoParams;
    bool _isChanged = false;
    bool _lockEvents = false;
    bool _firstChange = true;

    void createActions();
    void createToolbar(bool full);
    void editorChanged();
    void updateParamsEditorVisibility();

    friend class ElemFormulaWindow;
};

#endif // ELEM_FORMULA_EDITOR_H
