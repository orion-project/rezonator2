#include "PumpWindow.h"

#include "Appearance.h"
#include "PumpParamsDialog.h"
#include "funcs/FormatInfo.h"
#include "io/Clipboard.h"
#include "widgets/RichTextItemDelegate.h"
#include "widgets/ValuesEditorTS.h"

#include "widgets/OriOptionsGroup.h"
#include "widgets/OriStatusBar.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QToolButton>

enum PumpWindowStatusPanels
{
    STATUS_PUMPS_COUNT,
    STATUS_ACTIVE_PUMP,

    STATUS_PANELS_COUNT,
};

//------------------------------------------------------------------------------
//                             PumpWindowStorable
//------------------------------------------------------------------------------

namespace PumpWindowStorable
{

SchemaWindow* createWindow(Schema* schema)
{
    return PumpWindow::create(schema);
}

} // namespace PumpWindowStorable

//------------------------------------------------------------------------------
//                                PumpsTable
//------------------------------------------------------------------------------

PumpsTable::PumpsTable(Schema* schema, QWidget *parent) : QTableWidget(0, COL_COUNT, parent), _schema(schema)
{
    int paramsOffsetY = 0;
#if defined(Q_OS_MAC)
    paramsOffsetY = 2;
#elif defined(Q_OS_LINUX)
    paramsOffsetY = 1;
#elif defined(Q_OS_WIN)
    paramsOffsetY = 2;
#endif

    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegateForColumn(COL_PARAMS, new RichTextItemDelegate(paramsOffsetY, this));
    horizontalHeader()->setMinimumSectionSize(_iconSize+6);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(COL_IMAGE, _iconSize+6);
    horizontalHeader()->setSectionResizeMode(COL_ACTIVE, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(COL_ACTIVE, _iconSize+6);
    horizontalHeader()->setSectionResizeMode(COL_LABEL, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_PARAMS, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_TITLE, QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);
    setHorizontalHeaderLabels({ tr("Typ"), tr("On"), tr("Label"), tr("Params"), tr("Title") });

    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(doubleClicked(QTableWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    populate();
}

void PumpsTable::adjustColumns()
{
    resizeColumnToContents(COL_LABEL);
    resizeColumnToContents(COL_PARAMS);
}

void PumpsTable::doubleClicked(QTableWidgetItem*)
{
    PumpParams* pump = selected();
    if (pump) emit doubleClicked(pump);
}

void PumpsTable::showContextMenu(const QPoint& pos)
{
    if (_contextMenu)
        _contextMenu->popup(mapToGlobal(pos));
}

PumpParams* PumpsTable::selected() const
{
    return schema()->pumps()->at(currentRow());
}

QList<PumpParams*> PumpsTable::selection() const
{
    QList<PumpParams*> pumps;
    for (auto row : selectedRows())
        pumps << schema()->pumps()->at(row);
    return pumps;
}

QList<int> PumpsTable::selectedRows() const
{
    QList<int> rows;
    for (auto index : selectionModel()->selectedIndexes())
        if (!rows.contains(index.row()))
            rows << index.row();
    std::sort(rows.begin(), rows.end());
    return rows;
}

void PumpsTable::setSelected(PumpParams *param)
{
    setCurrentCell(findRow(param), 0);
}

void PumpsTable::populate()
{
    clearContents();
    setRowCount(schema()->pumps()->size());
    for (int row = 0; row < schema()->pumps()->size(); row++)
    {
        auto pump = schema()->pumps()->at(row);
        createRow(row);
        populateRow(pump, row);
    }
    adjustColumns();
}

void PumpsTable::createRow(int row)
{
    QTableWidgetItem *it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    setItem(row, COL_ACTIVE, it);

    it = new QTableWidgetItem();
    it->setFont(Z::Gui::PumpLabelFont().get());
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    setItem(row, COL_LABEL, it);

    it = new QTableWidgetItem();
    it->setFont(Z::Gui::ValueFont().get());
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_PARAMS, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_TITLE, it);
}

void PumpsTable::populateRow(PumpParams *pump, int row)
{
    auto pumpMode = Pumps::findByModeName(pump->modeName());
    if (pumpMode)
    {
        auto it = item(row, COL_IMAGE);
        it->setData(Qt::DecorationRole, QIcon(pumpMode->iconPath()).pixmap(_iconSize, _iconSize));
        it->setToolTip(pumpMode->displayName());
    }
    else
        qCritical() << "PumpsTable::populateRow(): Unable to find mode for pump parameters";

    auto iconPath = pump->isActive() ? ":/icons/pump_on" : ":/icons/pump_off";
    item(row, COL_ACTIVE)->setData(Qt::DecorationRole, QIcon(iconPath).pixmap(_iconSize, _iconSize));
    item(row, COL_LABEL)->setText(pump->label());
    item(row, COL_PARAMS)->setText(Z::Format::FormatPumpParams().format(pump));
    item(row, COL_TITLE)->setText(pump->title());
}

void PumpsTable::schemaLoaded(Schema*)
{
    populate();
}

void PumpsTable::pumpCreated(Schema*, PumpParams *pump)
{
    int row = rowCount();
    setRowCount(row+1);
    createRow(row);
    populateRow(pump, row);
    setSelected(pump);
}

void PumpsTable::pumpChanged(Schema*, PumpParams *pump)
{
    auto row = findRow(pump);
    if (row < 0) return;
    populateRow(reinterpret_cast<PumpParams*>(pump), row);
    adjustColumns();
}

void PumpsTable::pumpDeleting(Schema*, PumpParams *pump)
{
    auto row = findRow(pump);
    if (row < 0) return;
    removeRow(row);
    adjustColumns();
}

int PumpsTable::findRow(PumpParams *pump)
{
    return schema()->pumps()->indexOf(pump);
}

//------------------------------------------------------------------------------
//                                PumpWindow
//------------------------------------------------------------------------------

PumpWindow* PumpWindow::_instance = nullptr;

PumpWindow* PumpWindow::create(Schema* owner)
{
    if (!_instance)
        _instance = new PumpWindow(owner);
    return _instance;
}

PumpWindow::PumpWindow(Schema *owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Pumps"), ":/window_icons/pump");

    _table = new PumpsTable(owner);
    setContent(_table);

    createActions();
    createMenuBar();
    createToolBar();
    createStatusBar();

    _table->setContextMenu(_contextMenu);
    connect(_table, SIGNAL(doubleClicked(PumpParams*)), this, SLOT(editPump()));
    schema()->registerListener(_table);

    showStatusInfo();
}

PumpWindow::~PumpWindow()
{
    _instance = nullptr;

    schema()->unregisterListener(_table);
}

void PumpWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnPumpAdd = A_(tr("Create..."), this, SLOT(createPump()), ":/toolbar/pump_add", Qt::CTRL | Qt::Key_Insert);
    _actnPumpDelete = A_(tr("Delete..."), this, SLOT(deletePump()), ":/toolbar/pump_delete", Qt::CTRL | Qt::Key_Delete);
    _actnPumpEdit = A_(tr("Edit..."), this, SLOT(editPump()), ":/toolbar/pump_edit");
    _actnPumpActivate = A_(tr("Activate..."), this, SLOT(activatePump()), ":/toolbar/pump_on", Qt::CTRL | Qt::Key_Return);
    _actnPumpClone = A_(tr("Clone"), this, SLOT(clonePump()), ":/toolbar/clone");
    _actnPumpCopy = A_(tr("Copy"), this, SLOT(copy()), ":/toolbar/copy");
    _actnPumpPaste = A_(tr("Paste"), this, SLOT(paste()), ":/toolbar/paste");

    #undef A_
}

void PumpWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Pump"), this,
        { _actnPumpAdd, _actnPumpClone, nullptr, _actnPumpEdit, _actnPumpActivate, nullptr, _actnPumpDelete });

    _contextMenu = Ori::Gui::menu(this,
        { _actnPumpEdit, _actnPumpActivate, nullptr, _actnPumpClone, _actnPumpCopy, _actnPumpPaste, nullptr, _actnPumpDelete });
}

void PumpWindow::createToolBar()
{
    populateToolbar({
        Ori::Gui::textToolButton(_actnPumpAdd),
        Ori::Gui::textToolButton(_actnPumpClone),
        nullptr,
        Ori::Gui::textToolButton(_actnPumpEdit),
        Ori::Gui::textToolButton(_actnPumpActivate),
        nullptr,
        _actnPumpDelete
    });
}

void PumpWindow::createStatusBar()
{
    _statusBar = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    setContent(_statusBar);
}

PumpParams* PumpWindow::makeNewPumpDlg()
{
    auto pump = PumpParamsDialog::makeNewPump();
    if (!pump) return nullptr;
    if (AppSettings::instance().pumpAutoLabel)
        Z::Utils::generateLabel(schema(), pump);
    if (!PumpParamsDialog::editPump(pump))
    {
        delete pump;
        return nullptr;
    }
    return pump;
}

bool PumpWindow::editPumpDlg(PumpParams* pump)
{
    return PumpParamsDialog::editPump(pump);
}

PumpParams* PumpWindow::selectedPump() const
{
    return _table->selected();
}

void PumpWindow::addNewPump(PumpParams* pump)
{
    schema()->pumps()->append(pump);
    bool isFirstPump = schema()->pumps()->size() == 1;
    if (isFirstPump)
        pump->activate(true);
    schema()->events().raise(SchemaEvents::PumpCreated, pump, "PumpWindow: new pump created");
    if (isFirstPump)
        schema()->events().raise(SchemaEvents::RecalRequred, "PumpWindow: new pump created");
    showStatusInfo();
}

void PumpWindow::createPump()
{
    auto pump = makeNewPumpDlg();
    if (!pump) return;
    addNewPump(pump);
}

void PumpWindow::editPump()
{
    auto pump = selectedPump();
    if (!pump) return;
    if (!editPumpDlg(pump)) return;
    schema()->events().raise(SchemaEvents::PumpChanged, pump, "PumpWindow: pump params changed");
    if (pump->isActive() and schema()->tripType() == TripType::SP)
        schema()->events().raise(SchemaEvents::RecalRequred, "PumpWindow: pump params changed");
    showStatusInfo();
}

void PumpWindow::deletePump()
{
    if (schema()->isSP() && schema()->pumps()->size() == 1)
        return Ori::Dlg::info(tr("Unable to delete the last pump in SP schema."));

    auto pump = selectedPump();
    if (!pump) return;

    auto pumpId = pump->label().isEmpty()
            ? QString("#%1").arg(schema()->pumps()->indexOf(pump))
            : QString("'%1'").arg(pump->label());

    if (Ori::Dlg::ok(tr("Confirm deletion of pump %1").arg(pumpId)))
    {
        schema()->events().raise(SchemaEvents::PumpDeleting, pump, "PumpWindow: pump deleting");
        schema()->pumps()->removeOne(pump);
        schema()->events().raise(SchemaEvents::PumpDeleted, pump, "PumpWindow: pump deleted");
        if (pump->isActive())
        {
            if (schema()->pumps()->size() > 0)
            {
                auto pump = schema()->pumps()->first();
                pump->activate(true);
                schema()->events().raise(SchemaEvents::PumpChanged, pump, "PumpWindow: pump activated after active pump deleted");
            }
            schema()->events().raise(SchemaEvents::RecalRequred, "PumpWindow: pump activated after active pump deleted");
        }
        delete pump;
    }
    showStatusInfo();
}

void PumpWindow::activatePump()
{
    auto pump = selectedPump();
    if (!pump) return;

    if (pump->isActive()) return;

    auto oldPump = schema()->activePump();
    if (oldPump)
    {
        oldPump->activate(false);
        _table->pumpChanged(schema(), oldPump);
        schema()->events().raise(SchemaEvents::PumpChanged, oldPump, "PumpWindow: prev active pump deactivated");
    }

    pump->activate(true);
    _table->pumpChanged(schema(), pump);
    schema()->events().raise(SchemaEvents::PumpChanged, pump, "PumpWindow: another pump activated");

    if (schema()->tripType() == TripType::SP)
        schema()->events().raise(SchemaEvents::RecalRequred, "PumpWindow: another pump activated");

    showStatusInfo();
}

void PumpWindow::clonePump()
{
    auto pump = selectedPump();
    if (!pump) return;

    auto pumpMode = Pumps::findByModeName(pump->modeName());
    if (!pumpMode) return;

    auto newPump = pumpMode->makePump();
    if (AppSettings::instance().pumpAutoLabel)
        Z::Utils::generateLabel(schema(), newPump);
    else newPump->setLabel(pump->label());
    newPump->setTitle(pump->title());
    for (int i = 0; i < pump->params()->size(); i++)
        newPump->params()->byIndex(i)->setValue(pump->params()->at(i)->value());

    addNewPump(newPump);
}

void PumpWindow::showStatusInfo()
{
    _statusBar->setText(STATUS_PUMPS_COUNT, tr("Pumps: %1").arg(schema()->pumps()->size()));

    auto activePump = schema()->activePump();
    if (activePump)
    {
        QString activePumpStr = activePump->label();
        if (activePumpStr.isEmpty())
            activePumpStr = QString("#%1").arg(schema()->pumps()->indexOf(activePump)+1);
        _statusBar->setText(STATUS_ACTIVE_PUMP, tr("Active pump: %1").arg(activePumpStr));
    }
    else _statusBar->clear(STATUS_ACTIVE_PUMP);
}

bool PumpWindow::canCopy()
{
    return _table->selectionModel()->hasSelection();
}

void PumpWindow::selectAll()
{
    _table->selectAll();
}

void PumpWindow::copy()
{
    auto pumps = _table->selection();
    if (!pumps.isEmpty())
        Z::IO::Clipboard::setPumps(pumps);
}

static QString makeUniquePumpLabel(PumpParams* pump, const QList<PumpParams*>& allPumps)
{
    for (int index = 1; index < 1000; index++)
    {
        bool isUnique = true;
        auto label = QString("%1 (%2)").arg(pump->label()).arg(index);
        for (auto p : allPumps)
            if (p->label() == label)
            {
                isUnique = false;
                break;
            }
        if (isUnique)
            return label;
    }
    return pump->label();
}

void PumpWindow::paste()
{
    const auto pastedPumps = Z::IO::Clipboard::getPumps();
    const auto& existedPumps = *schema()->pumps();
    for (auto pastedPump : pastedPumps)
    {
        pastedPump->activate(false);

        for (auto existedPump : existedPumps)
            if (pastedPump->label() == existedPump->label())
                pastedPump->setLabel(makeUniquePumpLabel(pastedPump, existedPumps));

        addNewPump(pastedPump);
    }
}

void PumpWindow::shortcutEnterPressed()
{
   editPump();
}
