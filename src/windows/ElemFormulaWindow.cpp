#include "ElemFormulaWindow.h"

#include "../core/ElementFormula.h"
#include "../io/JsonUtils.h"
#include "../io/SchemaReaderJson.h"
#include "../io/SchemaWriterJson.h"
#include "../widgets/ElemFormulaEditor.h"

#include <QMenu>

//------------------------------------------------------------------------------
//                           ElemFormulaWindowStorable
//------------------------------------------------------------------------------

/// This class is used to reveal private constructor of @a ElemFormulaWindow
/// which is only should be used during loading.
class ElemFormulaWindowLoader
{
public:
    static SchemaWindow* createWindow(Schema* schema)
    {
        return new ElemFormulaWindow(schema);
    }
};

namespace ElemFormulaWindowStorable
{

SchemaWindow* createWindow(Schema* schema)
{
    return ElemFormulaWindowLoader::createWindow(schema);
}

} // namespace ElemFormulaWindowStorable

//------------------------------------------------------------------------------
//                             ElemFormulaWindow
//------------------------------------------------------------------------------

ElemFormulaWindow::ElemFormulaWindow(Schema *owner)
    : SchemaMdiChild(owner, InitOptions(initNoDefaultWidget | initNoToolBar))
{
    // This constructor is only called during schema loading,
    // so createContent() is called from storableRead() when the element gets known.
}

ElemFormulaWindow::ElemFormulaWindow(Schema *owner, ElemFormula *elem)
    : SchemaMdiChild(owner, InitOptions(initNoDefaultWidget | initNoToolBar))
{
    createContent(elem, nullptr);
    updateWindowTitle();
}

void ElemFormulaWindow::createContent(ElemFormula *sourceElem, ElemFormula *workingCopy)
{
    setWindowIcon(QIcon(":/elem_icon/ElemFormula"));

    _editor = new ElemFormulaEditor(sourceElem, workingCopy);
    connect(_editor, &ElemFormulaEditor::onModify, [this](){
        updateWindowTitle();
        // Mark schema as modified even if `_editor->isChanged` gets `false`.
        // It means the _editor was loaded having the state `changed = true`,
        // now it's been reset, and the schema becomes different
        // from the state in what it was loaded.
        schema()->markModified("ElemFormula edited");
    });
    connect(_editor, &ElemFormulaEditor::onApply, [this](){
        updateWindowTitle();
        schema()->events().raise(SchemaEvents::ElemChanged, _editor->sourceElem(), "ElemFormula apply");
        schema()->events().raise(SchemaEvents::RecalRequred, "ElemFormula apply");
    });

    _menuFormula = new QMenu(tr("Formula"));
    _editor->populateWindowMenu(_menuFormula);

    setContent(_editor);
}

void ElemFormulaWindow::updateWindowTitle()
{
    if (_editor->isChanged())
        setWindowTitle(tr("%1 (changed)").arg(_editor->sourceElem()->displayLabel()));
    else
        setWindowTitle(_editor->sourceElem()->displayLabel());
}

void ElemFormulaWindow::elementChanged(Schema*, Element* elem)
{
    if (elem == _editor->sourceElem())
        updateWindowTitle();
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
    auto sourceElem = dynamic_cast<ElemFormula*>(elem);
    if (!sourceElem)
    {
        report->warning(QString("Element at index %1 is not a formula element").arg(elemIndex));
        return false;
    }

    Z::IO::Json::JsonValue workingElemJson(root, "working_copy", report);
    if (!workingElemJson) return false;

    auto workingElem = Z::IO::Json::readElement(workingElemJson.obj(), report);
    if (!workingElem) return false;

    auto workingCopy = dynamic_cast<ElemFormula*>(workingElem);
    if (!workingCopy)
    {
        report->warning(QString("Working copy of element '%1' is not a formula ").arg(sourceElem->displayLabel()));
        return false;
    }

    createContent(sourceElem, workingCopy);
    _editor->setIsChanged(root["is_changed"].toBool());
    updateWindowTitle();
    return true;
}

bool ElemFormulaWindow::storableWrite(QJsonObject& root, Z::Report *report)
{
    Q_UNUSED(report)

    root["elem_index"] = schema()->indexOf(_editor->sourceElem());

    _editor->applyWorkingValues();
    _editor->resetModifyFlag();

    QJsonObject elemJson;
    Z::IO::Json::writeElement(elemJson, _editor->workingCopy());
    root["working_copy"] = elemJson;
    root["is_changed"] = _editor->isChanged();

    return true;
}
