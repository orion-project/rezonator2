#include "ElemFormulaWindow.h"

#include "../core/ElementFormula.h"
#include "../io/JsonUtils.h"
#include "../io/SchemaReaderJson.h"
#include "../io/SchemaWriterJson.h"
#include "../widgets/ElemFormulaEditor.h"

#include "helpers/OriDialogs.h"

#include <QCloseEvent>
#include <QMenu>
#include <QTimer>

//------------------------------------------------------------------------------
//                           ElemFormulaWindowStorable
//------------------------------------------------------------------------------

namespace ElemFormulaWindowStorable
{

SchemaWindow* createWindow(Schema* schema)
{
    return new ElemFormulaWindow(schema, nullptr);
}

} // namespace ElemFormulaWindowStorable

//------------------------------------------------------------------------------
//                             ElemFormulaWindow
//------------------------------------------------------------------------------

ElemFormulaWindow::ElemFormulaWindow(Schema *owner, ElemFormula *elem)
    : SchemaMdiChild(owner, InitOptions(initNoDefaultWidget | initNoToolBar))
{
    if (elem)
    {
        createContent(elem);
        updateWindowTitle();
    }
    // Otherwise, content will be created after the window gets loaded from file (see `storableRead()`)
}

void ElemFormulaWindow::closeEvent(QCloseEvent* ce)
{
    if (_forceClose | !_editor->isModified() ||
        Ori::Dlg::ok(tr("Element code has been changed.\nChanges will be lost if you close the window.")))
        SchemaMdiChild::closeEvent(ce);
    else
        ce->ignore();
}

void ElemFormulaWindow::createContent(ElemFormula *elem)
{
    setWindowIcon(QIcon(":/elem_icon/ElemFormula"));

    _editor = new ElemFormulaEditor(elem);
    connect(_editor, &ElemFormulaEditor::onModify, [this](){
        qDebug() << "HANDLER onModify";
        updateWindowTitle();
        // Mark schema as modified even if `_editor->isChanged` gets `false`.
        // This means that the _editor has applyed code to the element
        // so the editor is not in the "modified" state anymore
        // but the overall schema is "modified" now.
        schema()->markModified("ElemFormulaEditor::modified");
    });
    connect(_editor, &ElemFormulaEditor::onApply, [this](){
        updateWindowTitle();
        schema()->events().raise(SchemaEvents::ElemChanged, _editor->element(), "ElemFormulaEditor::apply");
        schema()->events().raise(SchemaEvents::RecalRequred, "ElemFormulaEditor::apply");
    });

    _menuFormula = new QMenu(tr("Formula"));
    _editor->populateWindowMenu(_menuFormula);

    setContent(_editor);
}

void ElemFormulaWindow::updateWindowTitle()
{
    if (_editor->isModified())
        setWindowTitle(tr("%1 (changed)").arg(_editor->element()->displayLabel()));
    else
        setWindowTitle(_editor->element()->displayLabel());
}

void ElemFormulaWindow::elementChanged(Schema*, Element* elem)
{
    if (elem == _editor->element())
        updateWindowTitle();
}

void ElemFormulaWindow::elementDeleting(Schema*, Element* elem)
{
    if (elem == _editor->element())
    {
        _forceClose = true;
        QTimer::singleShot(0, this, [this]{close();});
    }
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

bool ElemFormulaWindow::storableRead(const QJsonObject& root, Z::Report *report)
{
    int elemIndex = root["elem_index"].toInt();
    auto elem = schema()->element(elemIndex);
    if (!elem)
    {
        report->warning(QString("Invalid element index: %1").arg(elemIndex));
        return false;
    }
    auto elemFormula = dynamic_cast<ElemFormula*>(elem);
    if (!elemFormula)
    {
        report->warning(QString("Element at index %1 is not a formula element").arg(elemIndex));
        return false;
    }

    createContent(elemFormula);

    if (root.contains("code"))
        _editor->setCode(root["code"].toString());

    updateWindowTitle();
    return true;
}

bool ElemFormulaWindow::storableWrite(QJsonObject& root, Z::Report *report)
{
    Q_UNUSED(report)

    root["elem_index"] = schema()->indexOf(_editor->element());

    if (_editor->isModified())
        root["code"] = _editor->code();

    return true;
}
