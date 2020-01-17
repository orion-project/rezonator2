#include "SchemaViewWindow.h"

#include "AdjustmentWindow.h"
#include "ElementsCatalogDialog.h"
#include "ElementPropsDialog.h"
#include "CalcManager.h"
#include "CustomElemsManager.h"
#include "WindowsManager.h"
#include "core/ElementsCatalog.h"
#include "core/Utils.h"
#include "funcs_window/PlotFuncWindow.h"
#include "io/Clipboard.h"
#include "widgets/SchemaLayout.h"
#include "widgets/SchemaElemsTable.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QMenu>
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

    connect(_table, SIGNAL(doubleClicked(Element*)), this, SLOT(rowDoubleClicked(Element*)));
    connect(_table, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(currentCellChanged(int, int, int, int)));
    connect(_table, SIGNAL(beforeContextMenuShown(QMenu*)), this, SLOT(contextMenuAboutToShow(QMenu*)));
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

    actnElemAdd = A_(tr("Append..."), this, SLOT(actionElemAdd()), ":/toolbar/elem_add", Qt::CTRL | Qt::Key_Insert);
    actnElemMoveUp = A_(tr("Move Selected Up"), this, SLOT(actionElemMoveUp()), ":/toolbar/elem_move_up");
    actnElemMoveDown = A_(tr("Move Selected Down"), this, SLOT(actionElemMoveDown()), ":/toolbar/elem_move_down");
    actnElemProp = A_(tr("Properties..."), this, SLOT(actionElemProp()), ":/toolbar/elem_prop");
    actnElemMatr = A_(tr("Matrix"), _calculations, SLOT(funcShowMatrices()), ":/toolbar/elem_matr", Qt::SHIFT | Qt::Key_Return);
    actnElemMatrAll = A_(tr("Show All Matrices"), _calculations, SLOT(funcShowAllMatrices()));
    actnElemDelete = A_(tr("Delete..."), this, SLOT(actionElemDelete()), ":/toolbar/elem_delete", Qt::CTRL | Qt::Key_Delete);

    actnEditCopy = A_(tr("Copy", "Edit action"), this, SLOT(copy()), ":/toolbar/copy");
    actnEditPaste = A_(tr("Paste", "Edit action"), this, SLOT(paste()), ":/toolbar/paste");

    actnAdjuster = A_(tr("Add Adjuster"), this, SLOT(adjustParam()), ":/toolbar/adjust");

    actnSaveCustom = A_(tr("Save as Custom Element..."), this, SLOT(actionSaveCustom()), ":/toolbar/star");

    #undef A_
}

void SchemaViewWindow::createMenuBar()
{
    menuElement = Ori::Gui::menu(tr("Element"), this,
        { actnElemAdd, nullptr, actnElemMoveUp, actnElemMoveDown, nullptr, actnElemProp,
          actnElemMatr, actnElemMatrAll, nullptr, actnElemDelete, nullptr, actnSaveCustom });

    menuContextElement = Ori::Gui::menu(this,
        { actnElemProp, actnElemMatr, nullptr, actnAdjuster, nullptr,
          actnEditCopy, actnEditPaste, nullptr, actnElemDelete});

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
        schema()->events().raise(SchemaEvents::ElemChanged, elem, "SchemaViewWindow: element edited");
        schema()->events().raise(SchemaEvents::RecalRequred, "SchemaViewWindow: element edited");
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
    Element *elem = ElementsCatalogDialog::createElement();
    if (elem)
        schema()->insertElement(elem, _table->currentRow(), true);
}

void SchemaViewWindow::actionElemMoveUp()
{
    auto elem = _table->selected();
    if (elem)
    {
        schema()->moveElementUp(elem);
        _table->setSelected(elem);
    }
}

void SchemaViewWindow::actionElemMoveDown()
{
    auto elem = _table->selected();
    if (elem)
    {
        schema()->moveElementDown(elem);
        _table->setSelected(elem);
    }
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

void SchemaViewWindow::actionSaveCustom()
{
    Element* elem = _table->selected();
    if (elem)
        CustomElemsManager::saveAsCustom(schema(), elem);
}

//------------------------------------------------------------------------------
//                               Schema events

void SchemaViewWindow::elementCreated(Schema*, Element *elem)
{
    if (!_pasteMode && AppSettings::instance().elemAutoLabel)
    {
        // Disable elemChanged event from inside of elemCreated
        ElementLocker locker(elem, false);
        Z::Utils::generateLabel(schema(), elem);
    }
    if (!_pasteMode && AppSettings::instance().editNewElem)
        // All clients should process elementCreated event before elem will be changed,
        // so run deffered to avoid raise elementChanged from inside of elementCreated.
        QTimer::singleShot(0, [this, elem](){ this->editElement(elem); });
}

//------------------------------------------------------------------------------

bool SchemaViewWindow::canCopy()
{
    return _table->selectionModel()->hasSelection();
}

void SchemaViewWindow::copy()
{
    auto elems = _table->selection();
    if (!elems.isEmpty())
        Z::IO::Clipboard::setElements(elems);
}

void SchemaViewWindow::paste()
{
    auto elems = Z::IO::Clipboard::getElements();
    if (elems.isEmpty()) return;

    _pasteMode = true;
    bool doEvents = true;
    bool doLabels = AppSettings::instance().elemAutoLabelPasted;
    schema()->insertElements(elems, _table->currentRow(), doEvents, doLabels);
    _pasteMode = false;
}

void SchemaViewWindow::selectAll()
{
    _table->selectAll();
}

//------------------------------------------------------------------------------

void SchemaViewWindow::currentCellChanged(int curRow, int, int prevRow, int)
{
    int lastRow = _table->rowCount() - 1;
    if (curRow < lastRow && prevRow < lastRow) return;
    bool hasElem = curRow < lastRow;
    actnElemProp->setEnabled(hasElem);
    actnElemMatr->setEnabled(hasElem);
    actnElemMatrAll->setEnabled(hasElem);
    actnElemDelete->setEnabled(hasElem);
    actnEditCopy->setEnabled(hasElem);
    actnElemMoveUp->setEnabled(hasElem);
    actnElemMoveDown->setEnabled(hasElem);
    actnSaveCustom->setEnabled(hasElem);
}

void SchemaViewWindow::contextMenuAboutToShow(QMenu* menu)
{
    if (menu != menuContextElement) return;

    if (menuAdjuster)
    {
        delete menuAdjuster;
        menuAdjuster = nullptr;
    }
    actnAdjuster->setVisible(false);

    auto elem = _table->selected();
    if (!elem) return;

    auto params = Z::Utils::defaultParamFilter()->filter(elem->params());
    if (params.isEmpty()) return;

    if (params.size() == 1)
    {
        actnAdjuster->setVisible(true);
        actnAdjuster->setData(ptr2var(params.first()));
    }
    else
    {
        menuAdjuster = new QMenu(tr("Add Adjuster"));
        menuAdjuster->setIcon(QIcon(":/toolbar/adjust"));
        for (auto param : params)
        {
            auto action = menuAdjuster->addAction(param->label(), this, &SchemaViewWindow::adjustParam);
            action->setData(ptr2var(param));
        }
        menuContextElement->insertMenu(actnAdjuster, menuAdjuster);
    }
}

void SchemaViewWindow::adjustParam()
{
    auto param = var2ptr<Z::Parameter*>(qobject_cast<QAction*>(sender())->data());
    if (param) AdjustmentWindow::adjust(schema(), param);
}

void SchemaViewWindow::shortcutEnterPressed()
{
    if (_table->currentRow() == _table->rowCount() - 1)
        actionElemAdd();
    else
        actionElemProp();
}
