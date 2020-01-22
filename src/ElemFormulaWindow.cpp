#include "ElemFormulaWindow.h"

#include "core/ElementFormula.h"
#include "widgets/ElemFormulaEditor.h"

#include <QMenu>

ElemFormulaWindow::ElemFormulaWindow(Schema *owner, ElemFormula *elem)
    : SchemaMdiChild(owner, InitOptions(initNoDefaultWidget | initNoToolBar)), _element(elem)
{
    setTitleAndIcon(elem->displayLabel(), ":/elem_icon/ElemFormula");

    _editor = new ElemFormulaEditor(elem, false);

    _menuFormula = new QMenu(tr("Formula"));
    _editor->populateWindowMenu(_menuFormula);

    setContent(_editor);
}

bool ElemFormulaWindow::canCopy()
{
    return _editor->canCopy();
}

bool ElemFormulaWindow::canPaste()
{
    return _editor->canPaste();
}

void ElemFormulaWindow::selectAll()
{
    _editor->selectAll();
}

void ElemFormulaWindow::copy()
{
    _editor->copy();
}

void ElemFormulaWindow::paste()
{
    _editor->paste();
}
