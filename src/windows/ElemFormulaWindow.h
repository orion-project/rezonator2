#ifndef ELEM_FORMULA_WINDOW_H
#define ELEM_FORMULA_WINDOW_H

#include "../io/ISchemaWindowStorable.h"
#include "../windows/SchemaWindows.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

class ElemFormula;
class ElemFormulaEditor;

/**
    Implementation of restoreability for @a ElemFormulaWindow.
    Register it in @a ProjectWindow::registerStorableWindows().
*/
namespace ElemFormulaWindowStorable
{
const QString windowType("ElemFormulaWindow");
SchemaWindow* createWindow(Schema* schema);
}

/**
    The window wraps @a ElemFormulaEditor widget and allows showing it as MDI-child window.
    It also allows to store unfinshed formulas in schema file.
*/
class ElemFormulaWindow : public SchemaMdiChild,
                          public IEditableWindow,
                          public ISchemaWindowStorable
{
    Q_OBJECT

public:
    /// This constructor is used to show the window when user selects the "Edit formula" command.
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

    // inherits from SchemaListener
    void elementChanged(Schema*, Element*) override;

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return ElemFormulaWindowStorable::windowType; }
    bool storableRead(const QJsonObject& root, Z::Report* report) override;
    bool storableWrite(QJsonObject& root, Z::Report *report) override;

private:
    /// This contructor with one parameter is required to load window from schema file.
    explicit ElemFormulaWindow(Schema *owner);

    ElemFormulaEditor *_editor;
    QMenu *_menuFormula;

    void createContent(ElemFormula* sourceElem, ElemFormula *workingCopy);
    void updateWindowTitle();

    friend class ElemFormulaWindowLoader;
};

#endif // ELEM_FORMULA_WINDOW_H
