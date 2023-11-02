#include "CustomElemsWindow.h"

#include "CustomElemsManager.h"
#include "CustomPrefs.h"
#include "ElementsCatalogDialog.h"
#include "ElementPropsDialog.h"
#include "WindowsManager.h"
#include "core/ElementsCatalog.h"
#include "io/Clipboard.h"
#include "widgets/SchemaElemsTable.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriWindows.h"
#include "widgets/OriFlatToolBar.h"

#include <QFile>
#include <QFileSystemWatcher>
#include <QIcon>
#include <QMenu>
#include <QTabWidget>
#include <QToolButton>

namespace {

CustomElemsWindow* __instance = nullptr;

} // namespace

void CustomElemsWindow::showWindow()
{
    if (!__instance)
    {
        auto res = CustomElemsManager::loadLibrary();
        if (!res.ok())
        {
            Ori::Dlg::error(tr("There are messages while loading Custom Elements Library:\n\n%1").arg(res.error()));
            return;
        }
        __instance = new CustomElemsWindow(res.result());
    }
    __instance->show();
    __instance->activateWindow();
}

CustomElemsWindow::CustomElemsWindow(Schema *library) : QWidget(), _library(library)
{
    Ori::Wnd::initWindow(this, tr("Custom Elements Library"), ":/window_icons/catalog");

    createActions();
    createToolbar();
    createMenu();

    _table = new SchemaElemsTable(_library);
    _table->populate();
    connect(_table, QOverload<Element*>::of(&SchemaElemsTable::doubleClicked), this, &CustomElemsWindow::rowDoubleClicked);
    connect(_table, &SchemaElemsTable::currentCellChanged, this, &CustomElemsWindow::currentCellChanged);
    connect(_table, &SchemaElemsTable::beforeContextMenuShown, this, &CustomElemsWindow::contextMenuAboutToShow);
    _table->elementContextMenu = _menuContextElement;
    _table->lastRowContextMenu = _menuContextLastRow;
    _library->registerListener(_table);
    _library->selection().registerSelector(_table);

    auto elemsPage = Ori::Layouts::LayoutV({_toolbar, _table}).setMargin(2).setSpacing(0).makeWidget();

    _tabs = new QTabWidget;
    _tabs->addTab(elemsPage, tr("Elements"));

    Ori::Layouts::LayoutV({_tabs}).setMargin(3).useFor(this);

    _watcher = new QFileSystemWatcher(this);
    connect(_watcher, &QFileSystemWatcher::fileChanged, this, &CustomElemsWindow::libraryFileChanged);
    QString fileName = CustomElemsManager::libraryFile();
    if (QFile::exists(fileName))
        _watcher->addPath(fileName);

    restoreState();
    Ori::Wnd::moveToScreenCenter(this);
}

CustomElemsWindow::~CustomElemsWindow()
{
    storeState();
    __instance = nullptr;
    delete _library;
}

void CustomElemsWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnElemAdd = A_(tr("Append..."), this, SLOT(actionElemAdd()), ":/toolbar/elem_add", Qt::CTRL | Qt::Key_Insert);
    _actnElemMoveUp = A_(tr("Move Selected Up"), this, SLOT(actionElemMoveUp()), ":/toolbar/elem_move_up");
    _actnElemMoveDown = A_(tr("Move Selected Down"), this, SLOT(actionElemMoveDown()), ":/toolbar/elem_move_down");
    // TODO: share Enter shortcut between actions on different tabs (when those will be)
    _actnElemProp = A_(tr("Properties..."), this, SLOT(actionElemProp()), ":/toolbar/elem_prop", Qt::Key_Return);
    _actnElemDelete = A_(tr("Delete..."), this, SLOT(actionElemDelete()), ":/toolbar/elem_delete", Qt::CTRL | Qt::Key_Delete);

    // TODO: share edit shortcuts between actions on different tabs (when those will be)
    _actnEditCopy = A_(tr("Copy", "Edit action"), this, SLOT(copy()), ":/toolbar/copy", QKeySequence::Copy);
    _actnEditPaste = A_(tr("Paste", "Edit action"), this, SLOT(paste()), ":/toolbar/paste", QKeySequence::Paste);

    #undef A_
}

void CustomElemsWindow::createToolbar()
{
    _toolbar = new Ori::Widgets::FlatToolBar;
    _toolbar->setIconSize(Z::WindowUtils::toolbarIconSize());

    Ori::Gui::populate(_toolbar,
        { Ori::Gui::textToolButton(_actnElemAdd), nullptr, _actnElemMoveUp,
          _actnElemMoveDown, nullptr, _actnEditCopy, _actnEditPaste, nullptr,
          Ori::Gui::textToolButton(_actnElemProp), nullptr, _actnElemDelete });
}

void CustomElemsWindow::createMenu()
{
    _menuContextElement = Ori::Gui::menu(this,
        { _actnElemProp, nullptr,
          _actnEditCopy, _actnEditPaste, nullptr, _actnElemDelete});

    _menuContextLastRow = Ori::Gui::menu(this,
        { _actnElemAdd, _actnEditPaste });

}

void CustomElemsWindow::restoreState()
{
    QJsonObject root = CustomDataHelpers::loadCustomData("elems");

    CustomDataHelpers::restoreWindowSize(root, this, 750, 400);
}

void CustomElemsWindow::storeState()
{
    QJsonObject root;
    root["window_width"] = width();
    root["window_height"] = height();

    CustomDataHelpers::saveCustomData(root, "elems");
}

void CustomElemsWindow::actionElemAdd()
{
    Element *sample = ElementsCatalogDialog::chooseElementSample();
    if (!sample) return;

    QSharedPointer<Element> sampleDeleter;
    bool isCustom = sample->hasOption(Element_CustomSample);
    if (isCustom) sampleDeleter.reset(sample);

    Element *elem = ElementsCatalog::instance().create(sample, isCustom);
    if (!elem) return;

    _library->insertElements({elem}, _table->currentRow(), Arg::RaiseEvents(true));
    saveLibrary();
    editElement(elem);
}

void CustomElemsWindow::actionElemMoveUp()
{
    auto elem = _table->selected();
    if (elem)
    {
        _library->moveElementUp(elem);
        _table->setSelected(elem);
        saveLibrary();
    }
}

void CustomElemsWindow::actionElemMoveDown()
{
    auto elem = _table->selected();
    if (elem)
    {
        _library->moveElementDown(elem);
        _table->setSelected(elem);
        saveLibrary();
    }
}

void CustomElemsWindow::actionElemProp()
{
    auto elem = _table->selected();
    if (elem)
        editElement(elem);
    else
        // There will be no element when we press Enter on the last row
        // which is for element creation
        actionElemAdd();
}

void CustomElemsWindow::actionElemDelete()
{
    Elements elements = _table->selection();
    if (elements.isEmpty()) return;

    QStringList confirmation;
    confirmation << tr("Deleting elements:") << "";

    // List deleting elements
    for (int i = 0; i < elements.size(); i++)
        confirmation << QString("<b>%1</b>").arg(elements[i]->displayLabelTitle());

    if (not Ori::Dlg::ok(confirmation.join("<br>"))) return;

    _library->deleteElements(elements, Arg::RaiseEvents(true), Arg::FreeElem(true));

    saveLibrary();
}

void CustomElemsWindow::editElement(Element* elem)
{
    if (ElementPropsDialog::editElement(elem))
    {
        _library->events().raise(SchemaEvents::ElemChanged, elem, "CustomElemsWindow: element edited");
        saveLibrary();
    }
}

void CustomElemsWindow::rowDoubleClicked(Element* elem)
{
    if (elem)
        editElement(elem);
    else
        actionElemAdd();
}

void CustomElemsWindow::currentCellChanged(int curRow, int, int prevRow, int)
{
    int lastRow = _table->rowCount() - 1;
    if (curRow < lastRow && prevRow < lastRow) return;
    bool hasElem = curRow < lastRow;
    _actnElemDelete->setEnabled(hasElem);
    _actnEditCopy->setEnabled(hasElem);
    _actnElemMoveUp->setEnabled(hasElem);
    _actnElemMoveDown->setEnabled(hasElem);
}

void CustomElemsWindow::contextMenuAboutToShow(QMenu* menu)
{
    if (menu != _menuContextElement) return;

    auto elem = _table->selected();
    if (!elem) return;

    // TODO: check if element is formula and activate the 'Edit Formula' command
}

void CustomElemsWindow::copy()
{
    auto elems = _table->selection();
    if (!elems.isEmpty())
        Z::IO::Clipboard::setElements(elems);
}

void CustomElemsWindow::paste()
{
    auto elems = Z::IO::Clipboard::getElements();
    if (elems.isEmpty()) return;

    _library->insertElements(elems, _table->currentRow(), Arg::RaiseEvents(true));
    saveLibrary();
}

bool CustomElemsWindow::saveLibrary()
{
    _isLibrarySaving = true;
    QString res = CustomElemsManager::saveLibrary(_library);
    if (!res.isEmpty())
    {
        Ori::Dlg::error(tr("There are messages while saving Custom Elements Library:\n\n%1").arg(res));
        return false;
    }
    QString fileName = CustomElemsManager::libraryFile();
    if (!_watcher->files().contains(fileName))
        _watcher->addPath(fileName);
    return true;
}

void CustomElemsWindow::libraryFileChanged(const QString&)
{
    if (_isLibrarySaving)
    {
        _isLibrarySaving = false;
        return;
    }

    auto res = CustomElemsManager::loadLibrary();
    if (!res.ok())
    {
        Ori::Dlg::error(tr("There are messages while reloading Custom Elements Library:\n\n%1").arg(res.error()));
        return;
    }

    QSharedPointer<Schema> reloadedLibrary(res.result());

    QString fileName = CustomElemsManager::libraryFile();
    if (!QFile::exists(fileName))
    {
        _watcher->removePath(fileName);
        if (_library->count() > 0)
        {
            if (Ori::Dlg::yes(tr(
                 "It's detected that the Custom Elements Library "
                 "has been removed from the filesystem by some other process. "
                 "Do you want to save the current set of elements as a new library?")))
            {
                saveLibrary();
            }
            else
            {
                _library->deleteElements(_library->elements(), Arg::RaiseEvents(false), Arg::FreeElem(true));
                _table->populate();
            }
        }
        return;
    }

    // Move elements from reloaded library to the current library instance
    Elements reloadedElems(reloadedLibrary->elements());
    reloadedLibrary->deleteElements({reloadedElems}, Arg::RaiseEvents(false), Arg::FreeElem(false));
    _library->deleteElements(_library->elements(), Arg::RaiseEvents(false), Arg::FreeElem(true));
    _library->insertElements({reloadedElems}, -1, Arg::RaiseEvents(false));
    _table->populate();
}
