#include "ElementsCatalogDialog.h"
#include "ElementPropsDialog.h"
#include "SchemaViewWindow.h"
#include "CalcManager.h"
#include "WindowsManager.h"
#include "io/Clipboard.h"
#include "core/ElementsCatalog.h"
#include "funcs_window/PlotFuncWindow.h"
#include "widgets/SchemaLayout.h"
#include "widgets/SchemaElemsTable.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>

SchemaViewWindow::SchemaViewWindow(Schema *owner, CalcManager *calcs) : SchemaMdiChild(owner), _calculations(calcs)
{
    setTitleAndIcon(tr("Schema", "Window title"), ":/window_icons/schema");
    setAttribute(Qt::WA_DeleteOnClose, false);

    _table = new SchemaElemsTable(owner);
    _layout = new SchemaLayout(owner);

    setContent(Ori::Gui::splitterV(_table, _layout, 2, 20));

    createActions();
    createMenuBar();
    createToolBar();

    schema()->registerListener(_table);
    schema()->registerListener(_layout);
    schema()->selection().registerSelector(_table);

    connect(_table, SIGNAL(doubleClicked(Element*)), this, SLOT(actionElemProp()));
    _table->setContextMenu(menuContext);
}

SchemaViewWindow::~SchemaViewWindow()
{
    schema()->unregisterListener(_layout);
    schema()->unregisterListener(_table);
    schema()->selection().unregisterSelector(_table);
    delete _table;
    delete _layout;
}

void SchemaViewWindow::createActions()
{
    #define A_ Ori::Gui::action

    actnElemAdd = A_(tr("A&ppend..."), this, SLOT(actionElemAdd()), ":/toolbar/elem_add", Qt::CTRL | Qt::Key_Insert);
    actnElemInsertBefore = A_(tr("Create &Before Selection..."), this, SLOT(actionElemInsertBefore()), ":/toolbar/elem_insert_before", Qt::CTRL | Qt::SHIFT | Qt::Key_Insert);
    actnElemInsertAfter = A_(tr("Create &After Selection..."), this, SLOT(actionElemInsertAfter()), ":/toolbar/elem_insert_after", Qt::CTRL | Qt::ALT | Qt::Key_Insert);
    actnElemProp = A_(tr("&Properties..."), this, SLOT(actionElemProp()), ":/toolbar/elem_prop", Qt::Key_Return);
    actnElemMatr = A_(tr("&Matrix"), _calculations, SLOT(funcShowMatrices()), ":/toolbar/elem_matr", Qt::SHIFT | Qt::Key_Return);
    actnElemMatrAll = A_(tr("&Show All Matrices"), _calculations, SLOT(funcShowAllMatrices()));
    actnElemDelete = A_(tr("&Delete"), this, SLOT(actionElemDelete()), ":/toolbar/elem_delete", Qt::CTRL | Qt::Key_Delete);

    actnEditCopy = A_(tr("&Copy", "Edit action"), this, SLOT(copy()), ":/toolbar/copy");
    actnEditPaste = A_(tr("&Paste", "Edit action"), this, SLOT(paste()), ":/toolbar/paste");

    #undef A_
}

void SchemaViewWindow::createMenuBar()
{
    menuElement = Ori::Gui::menu(tr("E&lement"), this,
        { actnElemAdd, actnElemInsertBefore, actnElemInsertAfter, nullptr, actnElemProp,
          actnElemMatr, actnElemMatrAll, nullptr, actnElemDelete });

    menuContext = Ori::Gui::menu(this,
        { actnElemProp, actnElemMatr, nullptr, actnElemInsertBefore, actnElemInsertAfter, nullptr,
          actnEditCopy, actnEditPaste, nullptr, actnElemDelete });
}

void SchemaViewWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(actnElemAdd), actnElemInsertBefore,
                      actnElemInsertAfter, nullptr, Ori::Gui::textToolButton(actnElemProp),
                      actnElemMatr, nullptr, actnElemDelete });
}

void SchemaViewWindow::editElement(Element* elem)
{
    if (ElementPropsDialog::editElement(elem))
    {
        schema()->events().raise(SchemaEvents::ElemChanged, elem);
        schema()->events().raise(SchemaEvents::RecalRequred);
    }
}

//------------------------------------------------------------------------------
//                             Element actions

void SchemaViewWindow::actionElemAdd()
{
    Element *elem = Z::Dlgs::createElement();
    if (elem)
        schema()->insertElement(elem, -1, true);
}

void SchemaViewWindow::actionElemInsertBefore()
{
    Element *elem = Z::Dlgs::createElement();
    if (elem)
        schema()->insertElement(elem, _table->currentRow(), true);
}

void SchemaViewWindow::actionElemInsertAfter()
{
    Element *elem = Z::Dlgs::createElement();
    if (elem)
    {
        QVector<int> rows = _table->selectedRows();
        schema()->insertElement(elem, rows.isEmpty()? 0: rows.last()+1, true);
    }
}

void SchemaViewWindow::actionElemProp()
{
    if (_table->hasSelection())
        editElement(_table->selected());
}

void SchemaViewWindow::actionElemDelete()
{
    Elements elements = _table->selection();
    if (elements.isEmpty()) return;

    QStringList confirmation;
    confirmation << tr("Deleting elements:") << "";

    // List deleting elements
    for (int i = 0; i < elements.size(); i++)
        confirmation << QString("<b>%1</b>").arg(elements[i]->displayLabelTitle());

    // Check dependent windows
    bool warningAdded = false;
    for (auto window : WindowsManager::instance().schemaWindows(schema()))
    {
        auto plotWindow = dynamic_cast<PlotFuncWindow*>(window);
        if (!plotWindow) continue;
        auto reaction = plotWindow->reactElemDeletion(elements);
        if (reaction == ElemDeletionReaction::Close)
        {
            if (!warningAdded)
            {
                warningAdded = true;
                confirmation << ""
                             << tr("Some of the opened function windows")
                             << tr("depend on listed elements and will close:")
                             << "";
            }
            confirmation << QString("<b>%1</b>").arg(plotWindow->windowTitle());
        }
    }

    confirmation << "" <<  tr("Confirm deletion.");

    if (!Ori::Dlg::ok(confirmation.join("<br>"))) return;

    for (int i = 0; i < elements.size(); i++)
        schema()->deleteElement(elements[i], true);
}

//------------------------------------------------------------------------------
//                               Schema events

void SchemaViewWindow::elementCreated(Schema*, Element *elem)
{
    if (!_pasteMode && Settings::instance().elemAutoLabel)
    {
        // Disable elemChanged event from inside of elemCreated
        ElementLocker locker(elem, false);
        schema()->generateLabel(elem);
    }
    if (!_pasteMode && Settings::instance().editNewElem)
        // All clients should process elementCreated event before elem will be changed,
        // so run deffered to avoid raise elementChanged from inside of elementCreated.
        QTimer::singleShot(0, [this, elem](){ this->editElement(elem); });
}

//------------------------------------------------------------------------------

bool SchemaViewWindow::canCopy()
{
    return _table->hasSelection();
}

void SchemaViewWindow::copy()
{
    Z::IO::Clipboard::copyElements(_table->selection());
}

void SchemaViewWindow::paste()
{
    _pasteMode = true;
    Z::IO::Clipboard::pasteElements(schema());
    _pasteMode = false;
}
