#include "SchemaViewWindow.h"

#include "../app/CalcManager.h"
#include "../app/CustomElemsManager.h"
#include "../core/ElementsCatalog.h"
#include "../core/ElementFormula.h"
#include "../core/Utils.h"
#include "../funcs/PlotFuncWindow.h"
#include "../io/Clipboard.h"
#include "../widgets/ElementsTable.h"
#include "../widgets/SchemaLayout.h"
#include "../windows/AdjustmentWindow.h"
#include "../windows/ElemFormulaWindow.h"
#include "../windows/ElementsCatalogDialog.h"
#include "../windows/ElementPropsDialog.h"
#include "../windows/WindowsManager.h"

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

    _table = new ElementsTable(owner);
    _layout = new SchemaLayout(owner);

    setContent(Ori::Gui::splitterV(_table, _layout, 2, 20));

    createActions();
    createMenuBar();
    createToolBar();

    schema()->registerListener(_layout);
    schema()->selection().registerSelector(_table);

    connect(_table, &ElementsTable::elemDoubleClicked, this, &SchemaViewWindow::elemDoubleClicked);
    connect(_table, &ElementsTable::currentElemChanged, this, &SchemaViewWindow::currentElemChanged);
    connect(_table, &ElementsTable::selectedElemsChanged, _layout, &SchemaLayout::updateSelection);
    connect(_layout, &SchemaLayout::selectedElemsChanged, _table, &ElementsTable::selectElems);
    connect(_layout, &SchemaLayout::elemDoubleClicked, this, &SchemaViewWindow::elemDoubleClicked);
    _table->elementContextMenu = menuContextElement;
    _table->lastRowContextMenu = menuContextLastRow;
    _layout->elementContextMenu = menuContextElement;
    _layout->getSelection = [this]{ return _table->selection(); };
}

SchemaViewWindow::~SchemaViewWindow()
{
    schema()->unregisterListener(_layout);
    schema()->selection().unregisterSelector(_table);
    delete _table;
    delete _layout;
}

void SchemaViewWindow::createActions()
{
    #define A_ Ori::Gui::V0::action

    actnElemAdd = A_(tr("Append..."), this, SLOT(actionElemAdd()), ":/toolbar/elem_add", Qt::CTRL | Qt::Key_Insert);
    actnElemReplace = A_(tr("Replace..."), this, SLOT(actionElemReplace()), ":/toolbar/elem_replace");
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
    actnEditFormula = A_(tr("Edit Formula"), this, SLOT(actionEditFormula()), ":/toolbar/edit_formula");
    actnElemDisable = A_(tr("Disable/Enable"), this, SLOT(actionElemDisable()), ":/toolbar/switch_disabled");

    #undef A_
}

void SchemaViewWindow::createMenuBar()
{
    menuElement = Ori::Gui::menu(tr("Element"), this,
        { actnElemAdd, actnElemReplace, nullptr, actnElemMoveUp, actnElemMoveDown, nullptr, actnElemProp, actnEditFormula,
          actnElemMatr, actnElemMatrAll, nullptr, actnElemDisable, nullptr, actnElemDelete, nullptr, actnSaveCustom });

    menuContextElement = Ori::Gui::menu(this,
        { actnElemProp, actnEditFormula, actnElemMatr, nullptr, actnAdjuster, nullptr,
          actnEditCopy, actnEditPaste, nullptr, actnElemDisable, actnElemReplace, nullptr, actnElemDelete});
    connect(menuContextElement, &QMenu::aboutToShow, this, &SchemaViewWindow::elemsContextMenuAboutToShow);

    menuContextLastRow = Ori::Gui::menu(this,
        { actnElemAdd, actnEditPaste });
}

void SchemaViewWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(actnElemAdd), actnElemReplace, nullptr, actnElemMoveUp,
                      actnElemMoveDown, nullptr, Ori::Gui::textToolButton(actnElemProp),
                      actnElemMatr, nullptr, actnElemDisable, nullptr, actnElemDelete });
}

void SchemaViewWindow::editElement(Element* elem)
{
    bool wasDisabled = elem->disabled();
    if (ElementPropsDialog::editElement(elem))
    {
        if (wasDisabled != elem->disabled())
            schema()->relinkInterfaces();

        schema()->events().raise(SchemaEvents::ElemChanged, elem, "SchemaViewWindow: element edited");
        schema()->events().raise(SchemaEvents::RecalRequred, "SchemaViewWindow: element edited");
    }
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
    auto sample = ElementsCatalogDialog::chooseElementSample();
    if (!sample) return;

    Element* elem = ElementsCatalog::instance().create(sample->elem, sample->isCustom);
    if (!elem) return;

    if (!sample->isCustom && !Z::Utils::isInterface(elem))
        // For customs, matrix calculated after param values copied from sample
        // For interfaces, matrix calculated after insertion into schema
        elem->calcMatrix("SchemaViewWindow: element added");

    if (AppSettings::instance().elemAutoLabel)
        Z::Utils::generateLabel(schema()->elements(), elem, sample->isCustom ? sample->elem->label() : QString());

    schema()->insertElements({elem}, _table->currentRow(), Arg::RaiseEvents(true));
    _table->setCurrentElem(elem);

    if (AppSettings::instance().editNewElem)
        editElement(elem);
}

void SchemaViewWindow::actionElemReplace()
{
    auto curElem = _table->currentElem();
    if (!curElem) return;
    auto curRow = _table->currentRow();

    auto sample = ElementsCatalogDialog::chooseElementSample();
    if (!sample) return;

    if (sample->elem->type() == curElem->type())
    {
        if (sample->isCustom)
            Z::Utils::copyParamValues(sample->elem, curElem, "SchemaViewWindow::replaceElement");
        return;
    }

    if (not confirmDeletion({curElem}, true)) return;

    Element* elem = ElementsCatalog::instance().create(sample->elem, sample->isCustom);
    if (!elem) return;

    elem->setLabel(curElem->label());
    elem->setTitle(curElem->title());
    Z::Utils::copyParamValuesByName(curElem, elem, "SchemaViewWindow::replaceElement");

    schema()->deleteElements({curElem}, Arg::RaiseEvents(true), Arg::FreeElem(true));

    if (!sample->isCustom && !Z::Utils::isInterface(elem))
        // For customs, matrix calculated after param values copied from sample
        // For interfaces, matrix calculated after insertion into schema
        elem->calcMatrix("SchemaViewWindow: element added");

    schema()->insertElements({elem}, curRow, Arg::RaiseEvents(true));
    _table->setCurrentElem(elem);
}

void SchemaViewWindow::actionElemMoveUp()
{
    auto elem = _table->currentElem();
    if (elem)
    {
        schema()->moveElementUp(elem);
        _table->setCurrentElem(elem);
    }
}

void SchemaViewWindow::actionElemMoveDown()
{
    auto elem = _table->currentElem();
    if (elem)
    {
        schema()->moveElementDown(elem);
        _table->setCurrentElem(elem);
    }
}

void SchemaViewWindow::actionElemProp()
{
    auto elem = _table->selected();
    if (elem) editElement(elem);
}

bool SchemaViewWindow::confirmDeletion(const Elements &elements, bool onlyIfWarnings)
{
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

    if (onlyIfWarnings and not warningAdded)
        return true;

    confirmation << "" <<  tr("Confirm deletion.");
    return Ori::Dlg::ok(confirmation.join("<br>"));
}

void SchemaViewWindow::actionElemDelete()
{
    Elements elements = _table->selection();
    if (elements.isEmpty()) return;

    if (not confirmDeletion(elements, false)) return;

    // table selection is not ordered
    // deleting element will be in order of clicks
    // we try to select an element after the last clicked/marked for deletion
    auto nextElem = schema()->element(schema()->indexOf(elements.last())+1);

    schema()->deleteElements(elements, Arg::RaiseEvents(true), Arg::FreeElem(true));

    if (!nextElem)
        nextElem = schema()->element(schema()->count()-1);
    _table->setCurrentElem(nextElem);
}

void SchemaViewWindow::actionSaveCustom()
{
    Element* elem = _table->currentElem();
    if (!elem) return;

    QString res = CustomElemsManager::saveToLibrary(elem);
    if (!res.isEmpty())
        Ori::Dlg::error(res);
}

void SchemaViewWindow::actionEditFormula()
{
    auto elem = dynamic_cast<ElemFormula*>(_table->currentElem());
    if (!elem) return;

    WindowsManager::instance().show(new ElemFormulaWindow(schema(), elem));
}

void SchemaViewWindow::actionElemDisable()
{
    foreach (auto elem, _table->selection())
        elem->setDisabled(!elem->disabled());
    schema()->relinkInterfaces();
    schema()->events().raise(SchemaEvents::RecalRequred, "SchemaViewWindow: toggle disabled");
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

    schema()->insertElements(pastedElems, _table->currentRow(), Arg::RaiseEvents(true));
    _table->selectElems(pastedElems);
}

void SchemaViewWindow::selectAll()
{
    _table->selectAll();
}

//------------------------------------------------------------------------------

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
    actnElemDisable->setEnabled(hasElem);

    bool isFormula = dynamic_cast<ElemFormula*>(elem);
    actnEditFormula->setEnabled(isFormula);
    actnEditFormula->setVisible(isFormula);
}

void SchemaViewWindow::elemsContextMenuAboutToShow()
{
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
            // Don't use param->label() because menus do not support HTML formatting (e.g. n<sub>0</sub>)
            auto action = menuAdjuster->addAction(param->alias(), this, &SchemaViewWindow::adjustParam);
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
    if (_table->currentRow() == schema()->count())
        actionElemAdd();
    else
        actionElemProp();
}
