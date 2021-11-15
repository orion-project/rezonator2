#include "SchemaViewWindow.h"

#include "AdjustmentWindow.h"
#include "ElemFormulaWindow.h"
#include "ElementsCatalogDialog.h"
#include "ElementPropsDialog.h"
#include "CalcManager.h"
#include "CustomElemsManager.h"
#include "WindowsManager.h"
#include "core/ElementsCatalog.h"
#include "core/ElementFormula.h"
#include "core/Utils.h"
#include "funcs_window/PlotFuncWindow.h"
#include "io/Clipboard.h"
#include "widgets/ElementsTable.h"
#include "widgets/SchemaLayout.h"
#include "widgets/SchemaElemsTable.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriDialogs.h"

#include <QAction>
#include <QMenu>
#include <QSplitter>
#include <QToolButton>

SchemaViewWindow::SchemaViewWindow(Schema *owner, CalcManager *calcs) : SchemaMdiChild(owner), _calculations(calcs)
{
    setTitleAndIcon(tr("Schema", "Window title"), ":/window_icons/schema");
    setAttribute(Qt::WA_DeleteOnClose, false);

    _table = new SchemaElemsTable(owner);
    _table1 = new ElementsTable(owner);
    _layout = new SchemaLayout(owner);

    setContent(Ori::Gui::splitterV(_table1, _layout, 2, 20));

    createActions();
    createMenuBar();
    createToolBar();

    //schema()->registerListener(_table);
    schema()->registerListener(_table1);
    schema()->registerListener(_layout);
    schema()->selection().registerSelector(_table1);

    connect(_table, SIGNAL(doubleClicked(Element*)), this, SLOT(rowDoubleClicked(Element*)));
    connect(_table, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(currentCellChanged(int, int, int, int)));
    connect(_table, SIGNAL(beforeContextMenuShown(QMenu*)), this, SLOT(contextMenuAboutToShow(QMenu*)));
    _table->elementContextMenu = menuContextElement;
    _table->lastRowContextMenu = menuContextLastRow;

    connect(_table1, &ElementsTable::elemDoubleClicked, this, &SchemaViewWindow::elemDoubleClicked);
    connect(_table1, &ElementsTable::currentElemChanged, this, &SchemaViewWindow::currentElemChanged);
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
    actnElemMatr = A_(tr("Show Matrix"), _calculations, SLOT(funcShowMatrices()), ":/toolbar/elem_matr", Qt::SHIFT | Qt::Key_Return);
    actnElemMatrAll = A_(tr("Show All Matrices"), _calculations, SLOT(funcShowAllMatrices()));
    actnElemDelete = A_(tr("Delete..."), this, SLOT(actionElemDelete()), ":/toolbar/elem_delete", Qt::CTRL | Qt::Key_Delete);
    actnEditCopy = A_(tr("Copy", "Edit action"), this, SLOT(copy()), ":/toolbar/copy");
    actnEditPaste = A_(tr("Paste", "Edit action"), this, SLOT(paste()), ":/toolbar/paste");
    actnAdjuster = A_(tr("Add Adjuster"), this, SLOT(adjustParam()), ":/toolbar/adjust");
    actnSaveCustom = A_(tr("Save to Custom Library..."), this, SLOT(actionSaveCustom()), ":/toolbar/star");
    actnEditFormula = A_(tr("Edit formula"), this, SLOT(actionEditFormula()), ":/toolbar/edit_formula");

    #undef A_
}

void SchemaViewWindow::createMenuBar()
{
    menuElement = Ori::Gui::menu(tr("Element"), this,
        { actnElemAdd, nullptr, actnElemMoveUp, actnElemMoveDown, nullptr, actnElemProp, actnEditFormula,
          actnElemMatr, actnElemMatrAll, nullptr, actnElemDelete, nullptr, actnSaveCustom });

    menuContextElement = Ori::Gui::menu(this,
        { actnElemProp, actnEditFormula, actnElemMatr, nullptr, actnAdjuster, nullptr,
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
//    if (elem)
//        editElement(elem);
//    else
//        actionElemAdd();
}

void SchemaViewWindow::elemDoubleClicked(Element *elem)
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
    Element *sample = ElementsCatalogDialog::chooseElementSample();
    if (!sample) return;

    QSharedPointer<Element> sampleDeleter;
    bool isCustom = sample->hasOption(Element_CustomSample);
    if (isCustom) sampleDeleter.reset(sample);

    Element* elem = ElementsCatalog::instance().create(sample, isCustom);
    if (!elem) return;

    if (!isCustom && !Z::Utils::isInterface(elem))
        // For customs, matrix calculated after param values copied from sample
        // For interfaces, matrix calculated after insertion into schema
        elem->calcMatrix("SchemaViewWindow: element added");

    if (AppSettings::instance().elemAutoLabel)
        Z::Utils::generateLabel(schema()->elements(), elem, isCustom ? sample->label() : QString());

    schema()->insertElements({elem}, _table1->currentRow(), Arg::RaiseEvents(true));
    _table1->setCurrentElem(elem);

    if (AppSettings::instance().editNewElem)
        editElement(elem);
}

void SchemaViewWindow::actionElemMoveUp()
{
    auto elem = _table1->currentElem();
    if (elem)
    {
        schema()->moveElementUp(elem);
        _table1->setCurrentElem(elem);
    }
}

void SchemaViewWindow::actionElemMoveDown()
{
    auto elem = _table1->currentElem();
    if (elem)
    {
        schema()->moveElementDown(elem);
        _table1->setCurrentElem(elem);
    }
}

void SchemaViewWindow::actionElemProp()
{
    auto elem = _table1->selected();
    if (elem) editElement(elem);
}

void SchemaViewWindow::actionElemDelete()
{
    Elements elements = _table1->selection();
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

    if (not Ori::Dlg::ok(confirmation.join("<br>"))) return;

    // table selection is not ordered
    // deleting element will be in order or clicks
    // we try to select an element after the last clicked/marked for deletion
    auto nextElem = schema()->element(schema()->indexOf(elements.last())+1);

    schema()->deleteElements(elements, Arg::RaiseEvents(true), Arg::FreeElem(true));

    if (!nextElem)
        nextElem = schema()->element(schema()->count()-1);
    _table1->setCurrentElem(nextElem);
}

void SchemaViewWindow::actionSaveCustom()
{
    Element* elem = _table1->currentElem();
    if (!elem) return;

    QString res = CustomElemsManager::saveToLibrary(elem);
    if (!res.isEmpty())
        Ori::Dlg::error(res);
}

void SchemaViewWindow::actionEditFormula()
{
    auto elem = dynamic_cast<ElemFormula*>(_table1->currentElem());
    if (!elem) return;

    WindowsManager::instance().show(new ElemFormulaWindow(schema(), elem));
}

//------------------------------------------------------------------------------

bool SchemaViewWindow::canCopy()
{
    return _table1->selectionModel()->hasSelection();
}

void SchemaViewWindow::copy()
{
    auto elems = _table1->selection();
    if (!elems.isEmpty())
        Z::IO::Clipboard::setElements(elems);
}

void SchemaViewWindow::paste()
{
    auto pastedElems = Z::IO::Clipboard::getElements();
    if (pastedElems.isEmpty()) return;

    if (AppSettings::instance().elemAutoLabelPasted)
    {
        Elements allElems(schema()->elements());
        for (auto elem : pastedElems)
        {
            Z::Utils::generateLabel(allElems, elem);
            allElems << elem;
        }
    }

    schema()->insertElements(pastedElems, _table1->currentRow(), Arg::RaiseEvents(true));
    _table1->selectElems(pastedElems);
}

void SchemaViewWindow::selectAll()
{
    _table1->selectAll();
}

//------------------------------------------------------------------------------

void SchemaViewWindow::currentCellChanged(int curRow, int, int, int)
{
//    int lastRow = _table1->rowCount() - 1;
//    bool hasElem = curRow < lastRow;
//    Element* curElem = hasElem ? schema()->element(curRow) : nullptr;
//    actnElemProp->setEnabled(hasElem);
//    actnElemMatr->setEnabled(hasElem);
//    actnElemMatrAll->setEnabled(hasElem);
//    actnElemDelete->setEnabled(hasElem);
//    actnEditCopy->setEnabled(hasElem);
//    actnElemMoveUp->setEnabled(hasElem);
//    actnElemMoveDown->setEnabled(hasElem);
//    actnSaveCustom->setEnabled(hasElem);

//    bool isFormula = dynamic_cast<ElemFormula*>(curElem);
//    actnEditFormula->setEnabled(isFormula);
//    actnEditFormula->setVisible(isFormula);
}

void SchemaViewWindow::currentElemChanged(Element* elem)
{
    bool hasElem = elem;
    actnElemProp->setEnabled(hasElem);
    actnElemMatr->setEnabled(hasElem);
    actnElemMatrAll->setEnabled(hasElem);
    actnElemDelete->setEnabled(hasElem);
    actnEditCopy->setEnabled(hasElem);
    actnElemMoveUp->setEnabled(hasElem);
    actnElemMoveDown->setEnabled(hasElem);
    actnSaveCustom->setEnabled(hasElem);

    bool isFormula = dynamic_cast<ElemFormula*>(elem);
    actnEditFormula->setEnabled(isFormula);
    actnEditFormula->setVisible(isFormula);
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
