#ifndef ELEM_FORMULA_EDITOR_H
#define ELEM_FORMULA_EDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
class QTextEdit;
class QToolBar;
QT_END_NAMESPACE

class ElemFormula;

namespace Ori::Widgets {
class CodeEditor;
}

class ElemFormulaEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ElemFormulaEditor(ElemFormula* elem);

    bool canCopy();
    bool canPaste();
    void selectAll();
    void copy();
    void paste();

    void populateWindowMenu(QMenu* menu);

    ElemFormula* element() { return _element; }

    bool isModified() const;
    QString code() const;
    void setCode(const QString &code);

signals:
    void onModify();
    void onApply();
    
private:
    ElemFormula* _element;
    QAction *_actnApplyCode, *_actnClearLog, *_actnShowHelp;
    Ori::Widgets::CodeEditor *_codeEditor;
    QTextEdit *_logView;
    QToolBar *_toolbar;
    bool _lockEvents = false;

    void createActions();
    void createToolbar();
    void applyFormula();
    void clearLog();
    void showHelp();
    void codeModified();
};

#endif // ELEM_FORMULA_EDITOR_H
