#ifndef ELEM_FORMULA_WINDOW_H
#define ELEM_FORMULA_WINDOW_H

#include "SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

class ElemFormula;
class ElemFormulaEditor;

class ElemFormulaWindow : public SchemaMdiChild, public IEditableWindow
{
    Q_OBJECT

public:
    explicit ElemFormulaWindow(Schema *owner, ElemFormula *elem);

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _menuFormula }; }

    // inherits from IEditableWindow
    SupportedCommands supportedCommands() override {
        return EditCmd_Copy | EditCmd_Paste | EditCmd_SelectAll; }
    bool canCopy() override;
    bool canPaste() override;
    void selectAll() override;
    void copy() override;
    void paste() override;

private:
    ElemFormula* _element;
    ElemFormulaEditor *_editor;
    QMenu *_menuFormula;
};

#endif // ELEM_FORMULA_WINDOW_H
