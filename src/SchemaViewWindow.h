#ifndef SCHEMA_WINDOW_H
#define SCHEMA_WINDOW_H

#include "SchemaWindows.h"
#include "core/Schema.h"
#include "core/Element.h"

#include <QCloseEvent>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

class CalcManager;

/**
    Basic window containing visual representation of schema (element list, layout)
    and holding actions controlling the schema (append, remove elements, etc.).
*/
class SchemaViewWindow: public SchemaMdiChild, public IEditableWindow, public IShortcutListener
{
    Q_OBJECT

public:
    SchemaViewWindow(Schema*, CalcManager*);
    ~SchemaViewWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { menuElement }; }

    // inherits from IEditableWindow
    SupportedCommands supportedCommands() override {
        return EditCmd_Copy | EditCmd_Paste | EditCmd_SelectAll; }
    bool canCopy() override;
    bool canPaste() override { return true; }
    void selectAll() override;

    // inherits from IShortcutListener
    void shortcutEnterPressed() override;

public slots:
    void copy() override;
    void paste() override;

protected:
    // SchemaViewWindow is always visible and can't be closed.
    void closeEvent(QCloseEvent *event) override { event->ignore(); }

private:
    QAction *actnElemAdd, *actnElemMoveUp, *actnElemMoveDown, *actnElemProp,
            *actnElemMatr, *actnElemMatrAll, *actnElemDelete, *actnEditCopy, *actnEditPaste,
            *actnAdjuster, *actnSaveCustom, *actnEditFormula;

    QMenu *menuElement, *menuContextElement, *menuContextLastRow;
    QMenu *menuAdjuster = nullptr;

    class SchemaLayout *_layout;
    class ElementsTable* _table;
    CalcManager* _calculations;

    void createActions();
    void createMenuBar();
    void createToolBar();

    void editElement(Element* elem);

private slots:
    void actionElemAdd();
    void actionElemMoveUp();
    void actionElemMoveDown();
    void actionElemProp();
    void actionElemDelete();
    void actionSaveCustom();
    void actionEditFormula();
    void elemDoubleClicked(Element*);
    void currentElemChanged(Element* elem);
    void contextMenuAboutToShow(QMenu* menu);
    void adjustParam();
};

#endif // SCHEMA_WINDOW_H
