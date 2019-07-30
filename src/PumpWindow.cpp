#include "PumpWindow.h"

#include "PumpParamsDialog.h"
#include "funcs/FormatInfo.h"
#include "widgets/Appearance.h"
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
    Z::PumpParams* pump = selected();
    if (pump) emit doubleClicked(pump);
}

void PumpsTable::showContextMenu(const QPoint& pos)
{
    if (_contextMenu)
        _contextMenu->popup(mapToGlobal(pos));
}

Z::PumpParams* PumpsTable::selected() const
{
    return schema()->pumps()->at(currentRow());
}

void PumpsTable::setSelected(Z::PumpParams *param)
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

void PumpsTable::populateRow(Z::PumpParams *pump, int row)
{
    auto pumpMode = Z::Pump::findByModeName(pump->modeName());
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
    item(row, COL_PARAMS)->setText(Z::Format::pumpParamsWithValues(pump));
    item(row, COL_TITLE)->setText("  " % pump->title());
}

void PumpsTable::schemaLoaded(Schema*)
{
    populate();
}

void PumpsTable::pumpCreated(Schema*, Z::PumpParams *pump)
{
    int row = rowCount();
    setRowCount(row+1);
    createRow(row);
    populateRow(pump, row);
    setSelected(pump);
}

void PumpsTable::pumpChanged(Schema*, Z::PumpParams *pump)
{
    auto row = findRow(pump);
    if (row < 0) return;
    populateRow(reinterpret_cast<Z::PumpParams*>(pump), row);
    adjustColumns();
}

void PumpsTable::pumpDeleting(Schema*, Z::PumpParams *pump)
{
    auto row = findRow(pump);
    if (row < 0) return;
    removeRow(row);
    adjustColumns();
}

int PumpsTable::findRow(Z::PumpParams *pump)
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
    connect(_table, SIGNAL(doubleClicked(Z::PumpParams*)), this, SLOT(editPump()));
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

    _actnPumpAdd = A_(tr("&Create..."), this, SLOT(createPump()), ":/toolbar/pump_add", Qt::CTRL | Qt::Key_Insert);
    _actnPumpDelete = A_(tr("&Delete"), this, SLOT(deletePump()), ":/toolbar/pump_delete", Qt::CTRL | Qt::Key_Delete);
    _actnPumpEdit = A_(tr("&Edit..."), this, SLOT(editPump()), ":/toolbar/pump_edit", Qt::Key_Return);
    _actnPumpActivate = A_(tr("&Activate..."), this, SLOT(activatePump()), ":/toolbar/pump_on", Qt::CTRL | Qt::Key_Return);
    _actnPumpClone = A_(tr("C&lone"), this, SLOT(clonePump()), ":/toolbar/clone");

    #undef A_
}

void PumpWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("&Pump"), this,
        { _actnPumpAdd, _actnPumpClone, nullptr, _actnPumpEdit, _actnPumpActivate, nullptr, _actnPumpDelete });

    _contextMenu = Ori::Gui::menu(this,
        { _actnPumpEdit, _actnPumpActivate, nullptr, _actnPumpDelete });
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

Z::PumpParams* PumpWindow::makeNewPumpDlg()
{
    auto pump = PumpParamsDialog::makeNewPump();
    if (!pump) return nullptr;
    if (Settings::instance().pumpAutoLabel)
        Z::Utils::generateLabel(schema(), pump);
    if (!PumpParamsDialog::editPump(pump))
    {
        delete pump;
        return nullptr;
    }
    return pump;
}

bool PumpWindow::editPumpDlg(Z::PumpParams* pump)
{
    return PumpParamsDialog::editPump(pump);
}

Z::PumpParams* PumpWindow::selectedPump() const
{
    return _table->selected();
}

void PumpWindow::addNewPump(Z::PumpParams* pump)
{
    schema()->pumps()->append(pump);
    bool isFirstPump = schema()->pumps()->size() == 1;
    if (isFirstPump)
        pump->activate(true);
    schema()->events().raise(SchemaEvents::PumpCreated, pump);
    if (isFirstPump)
        schema()->events().raise(SchemaEvents::RecalRequred);
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
    schema()->events().raise(SchemaEvents::PumpChanged, pump);
    if (pump->isActive() and schema()->tripType() == TripType::SP)
        schema()->events().raise(SchemaEvents::RecalRequred);
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
        schema()->events().raise(SchemaEvents::PumpDeleting, pump);
        schema()->pumps()->removeOne(pump);
        schema()->events().raise(SchemaEvents::PumpDeleted, pump);
        if (pump->isActive())
        {
            if (schema()->pumps()->size() > 0)
            {
                auto pump = schema()->pumps()->first();
                pump->activate(true);
                schema()->events().raise(SchemaEvents::PumpChanged, pump);
            }
            schema()->events().raise(SchemaEvents::RecalRequred);
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
        schema()->events().raise(SchemaEvents::PumpChanged, oldPump);
    }

    pump->activate(true);
    _table->pumpChanged(schema(), pump);
    schema()->events().raise(SchemaEvents::PumpChanged, pump);

    if (schema()->tripType() == TripType::SP)
        schema()->events().raise(SchemaEvents::RecalRequred);

    showStatusInfo();
}

void PumpWindow::clonePump()
{
    auto pump = selectedPump();
    if (!pump) return;

    auto pumpMode = Z::Pump::findByModeName(pump->modeName());
    if (!pumpMode) return;

    auto newPump = pumpMode->makePump();
    if (Settings::instance().pumpAutoLabel)
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
