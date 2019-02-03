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
#include <QShortcut>
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

    connect(_table, SIGNAL(doubleClicked(Element*)), this, SLOT(rowDoubleClicked(Element*)));
    connect(_table, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(currentCellChanged(int, int, int, int)));
    _table->elementContextMenu = menuContextElement;
    _table->lastRowContextMenu = menuContextLastRow;
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
    actnElemMoveUp = A_(tr("Move Selected &Up"), this, SLOT(actionElemMoveUp()), ":/toolbar/elem_insert_before");
    actnElemMoveDown = A_(tr("Move Selected &Down"), this, SLOT(actionElemMoveDown()), ":/toolbar/elem_insert_after");
    actnElemProp = A_(tr("&Properties..."), this, SLOT(actionElemProp()), ":/toolbar/elem_prop", Qt::Key_Return);
    actnElemMatr = A_(tr("&Matrix"), _calculations, SLOT(funcShowMatrices()), ":/toolbar/elem_matr", Qt::SHIFT | Qt::Key_Return);
    actnElemMatrAll = A_(tr("&Show All Matrices"), _calculations, SLOT(funcShowAllMatrices()));
    actnElemDelete = A_(tr("&Delete"), this, SLOT(actionElemDelete()), ":/toolbar/elem_delete", Qt::CTRL | Qt::Key_Delete);

    actnEditCopy = A_(tr("&Copy", "Edit action"), this, SLOT(copy()), ":/toolbar/copy");
    actnEditPaste = A_(tr("&Paste", "Edit action"), this, SLOT(paste()), ":/toolbar/paste");

    shortcutAddFromLastRow = new QShortcut(Qt::Key_Return, this);
    connect(shortcutAddFromLastRow, SIGNAL(activated()), this, SLOT(actionElemAdd()));

    #undef A_
}

void SchemaViewWindow::createMenuBar()
{
    menuElement = Ori::Gui::menu(tr("E&lement"), this,
        { actnElemAdd, nullptr, actnElemMoveUp, actnElemMoveDown, nullptr, actnElemProp,
          actnElemMatr, actnElemMatrAll, nullptr, actnElemDelete });

    menuContextElement = Ori::Gui::menu(this,
        { actnElemProp, actnElemMatr, nullptr,
          actnEditCopy, actnEditPaste, nullptr, actnElemDelete });

    menuContextLastRow = Ori::Gui::menu(this,
        { actnElemAdd, actnEditPaste });
}

void SchemaViewWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(actnElemAdd), nullptr, actnElemMoveUp,
                      actnElemMoveDown, nullptr, Ori::Gui::textToolButton(actnElemProp),
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

void SchemaViewWindow::rowDoubleClicked(Element *elem)
{
    if (elem)
        editElement(elem);
    else
        actionElemAdd();
}

//------------------------------------------------------------------------------
//                             Element actions

void SchemaViewWindow::actionElemAdd()
{
    Element *elem = Z::Dlgs::createElement();
    if (elem)
        schema()->insertElement(elem, _table->currentRow(), true);
}

void SchemaViewWindow::actionElemMoveUp()
{
    // TODO
}

void SchemaViewWindow::actionElemMoveDown()
{
    // TODO
}

void SchemaViewWindow::actionElemProp()
{
    auto elem = _table->selected();
    if (elem) editElement(elem);
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

void SchemaViewWindow::copy()
{
    Z::IO::Clipboard::setElements(_table->selection());
}

void SchemaViewWindow::paste()
{
    auto elems = Z::IO::Clipboard::getElements();
    if (elems.isEmpty()) return;

    _pasteMode = true;
    bool doEvents = true;
    bool doLabels = Settings::instance().elemAutoLabelPasted;
    schema()->insertElements(elems, _table->currentRow(), doEvents, doLabels);
    _pasteMode = false;
}

void SchemaViewWindow::currentCellChanged(int currentRow, int, int, int)
{
    bool hasElem = currentRow < _table->rowCount() - 1;
    actnElemProp->setEnabled(hasElem);
    actnElemMatr->setEnabled(hasElem);
    actnElemDelete->setEnabled(hasElem);
    actnEditCopy->setEnabled(hasElem);
    actnElemMoveUp->setEnabled(hasElem);
    actnElemMoveDown->setEnabled(hasElem);
    shortcutAddFromLastRow->setEnabled(!hasElem);
}
