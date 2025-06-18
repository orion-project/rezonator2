#include "TableFuncWindow.h"

#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../core/Format.h"
#include "../funcs/FuncWindowHelpers.h"
#include "../math/InfoFunctions.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/UnitWidgets.h"

#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriStatusBar.h"

#include <QAction>
#include <QClipboard>
#include <QHeaderView>
#include <QTextBrowser>
#include <QMenu>
#include <QPainter>
#include <QToolBar>

using namespace Ori::Gui::V0;

static const QIcon& resultPosIcon(TableFunction::ResultPosition pos)
{
    static QMap<TableFunction::ResultPosition, QIcon> icons;
    if (!icons.contains(pos))
        icons[pos] = QIcon(TableFunction::resultPositionInfo(pos).icon_path);
    return icons[pos];
}

//------------------------------------------------------------------------------
//                        TableFuncPositionColumnItemDelegate
//------------------------------------------------------------------------------

TableFuncPositionColumnItemDelegate::TableFuncPositionColumnItemDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void TableFuncPositionColumnItemDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    _paintingIndex = index;
    QItemDelegate::paint(painter, option, index);
}

void TableFuncPositionColumnItemDelegate::drawDisplay(
        QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const
{
    Q_ASSERT(_paintingIndex.isValid());

    QRect r = rect;
    r.setLeft(rect.left() + 3);

    QItemDelegate::drawDisplay(painter, option, r, text);

    auto resultPosition = TableFunction::ResultPosition(_paintingIndex.data(Qt::UserRole).toInt());
    resultPosIcon(resultPosition).paint(painter, r.right() - 26, r.top() + 2, 24, 24);
}

//------------------------------------------------------------------------------
//                             TableFuncResultTable
//------------------------------------------------------------------------------

enum FixedCols {
    COL_POSITION,

    FIXED_COLS_COUNT
};

TableFuncResultTable::TableFuncResultTable(TableFunction *func) : QTableWidget(), _function(func)
{
    setWordWrap(false);
    setColumnCount(FIXED_COLS_COUNT + _function->columnCount());
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    setItemDelegateForColumn(COL_POSITION, new TableFuncPositionColumnItemDelegate(this));

    connect(this, &QTableWidget::customContextMenuRequested, this, &TableFuncResultTable::showTableContextMenu);
    connect(horizontalHeader(), &QTableWidget::customContextMenuRequested, this, &TableFuncResultTable::showHeaderContextMenu);
    
}

void TableFuncResultTable::updateColumnTitles()
{
    QStringList titles;
    titles << tr("Position");
    for (auto& col : _function->columns())
    {
        QString title;
        if (showT and showS)
            title = QStringLiteral("%1 %2 %3 ").arg(col.titleT).arg(Z::Strs::multX()).arg(col.titleS);
        else if (showT)
            title = col.titleT;
        else
            title = col.titleS;
        if (col.unit != Z::Units::none())
        {
            if (!title.isEmpty())
                title += QStringLiteral(", ");
            title += col.unit->name();
        }
        titles << title;
    }
    setHorizontalHeaderLabels(titles);
}

void TableFuncResultTable::updateResults()
{
    const QVector<TableFunction::Result>& results = _function->results();

    if (results.isEmpty())
    {
        clearContents();
        return;
    }

    setRowCount(results.size());

    for (int row = 0; row < results.size(); row++)
    {
        const auto& res = results.at(row);

        QTableWidgetItem *it = item(row, COL_POSITION);
        if (!it)
        {
            it = new QTableWidgetItem();
            it->setFont(Z::Gui::ElemLabelFont().get());
            it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            setItem(row, COL_POSITION, it);
        }
        it->setText(res.element->displayLabel());
        it->setData(Qt::UserRole, int(res.position));
        it->setToolTip(TableFunction::resultPositionInfo(res.position).tooltip);

        const auto columns = _function->columns();
        for (int index = 0; index < res.values.size(); index++)
        {
            const auto& column = columns.at(index);
            const auto& value = res.values.at(index);
            double valueT = column.unit->fromSi(value.T);
            double valueS = column.unit->fromSi(value.S);

            QString valueStr;
            if (qIsNaN(valueT) || qIsNaN(valueS))
                valueStr = QStringLiteral("N/A");
            else if (showT and showS)
                valueStr = QStringLiteral("%1 %2 %3 ")
                        .arg(Z::format(valueT))
                        .arg(Z::Strs::multX())
                        .arg(Z::format(valueS));
            else if (showT)
                valueStr = Z::format(valueT);
            else
                valueStr = Z::format(valueS);

            it = item(row, FIXED_COLS_COUNT + index);
            if (!it)
            {
                it = new QTableWidgetItem();
                it->setFont(Z::Gui::ValueFont().get());
                it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
                it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                setItem(row, FIXED_COLS_COUNT + index, it);
            }
            it->setText(valueStr);
        }
    }
}

void TableFuncResultTable::showTableContextMenu(const QPoint& pos)
{
    if (!_contextMenu)
    {
        _contextMenu = new QMenu;
        _contextMenu->addAction(QIcon(":/toolbar/copy"), tr("Copy"), this, &TableFuncResultTable::copy);
        _contextMenu->addSeparator();
        _contextMenu->addAction(tr("Select All"), this, &QTableWidget::selectAll);
    }
    _contextMenu->popup(mapToGlobal(pos));
}

void TableFuncResultTable::showHeaderContextMenu(const QPoint& pos)
{
    // The first table column is "Position", not a result
    int colIndex = horizontalHeader()->logicalIndexAt(pos) - 1;
    const auto cols = _function->columns();
    if (colIndex < 0 || colIndex >= cols.size()) return;
    const auto& col = cols.at(colIndex);
    if (col.unit == Z::Units::none()) return;
    if (!_unitsMenu) {
        _unitsMenu = new UnitsMenu(this);
        connect(_unitsMenu, &UnitsMenu::unitChanged, this, [this](Z::Unit unit){
            auto colId = _unitsMenu->property("colId").toString();
            auto oldUnit = _unitsMenu->property("oldUnit").toString();
            if (oldUnit == unit->alias()) return;
            _function->schema()->markModified("TableFuncResultTable: unit changed");
            _function->setColumnUnit(colId, unit);
            updateColumnTitles();
            updateResults();
        });
    }
    _unitsMenu->setUnit(col.unit);
    _unitsMenu->setProperty("colId", col.id);
    _unitsMenu->setProperty("oldUnit", col.unit->alias());
    _unitsMenu->menu()->popup(mapToGlobal(pos));
}

void TableFuncResultTable::copy()
{
    auto ranges = selectedRanges();
    if (ranges.isEmpty()) return;
    QString report;
    QTextStream stream(&report);
    auto range = ranges.first();
    for (int row = range.topRow(); row <= range.bottomRow(); row++)
    {
        for (int col = range.leftColumn(); col <= range.rightColumn(); col++)
        {
            auto item = this->item(row, col);
            stream << item->text();
            if (col == 0)
            {
                auto resultPosition = TableFunction::ResultPosition(item->data(Qt::UserRole).toInt());
                stream << '\t' << TableFunction::resultPositionInfo(resultPosition).ascii;
            }
            if (col < range.rightColumn())
                stream << '\t';
        }
        stream << '\n';
    }
    qApp->clipboard()->setText(report);
}

//------------------------------------------------------------------------------
//                                TableFuncWindow
//------------------------------------------------------------------------------

enum PlotWindowStatusPanels
{
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

TableFuncWindow::TableFuncWindow(TableFunction* func) : SchemaMdiChild(func->schema()), _function(func)
{
    setTitleAndIcon(FuncWindowHelpers::makeWindowTitle(func), function()->iconPath());

    createContent();
    createActions();
    createMenuBar();
    createToolBar();
    createStatusBar();
}

TableFuncWindow::~TableFuncWindow()
{
    delete _function;
}

void TableFuncWindow::createActions()
{
    _actnUpdate = action(tr("Update"), this, SLOT(update()), ":/toolbar/update", Qt::Key_F5);

    _actnShowT = action(tr("Show T-plane"), this, SLOT(activateModeT()), ":/toolbar/plot_t");
    _actnShowS = action(tr("Show S-plane"), this, SLOT(activateModeS()), ":/toolbar/plot_s");
    _actnShowT->setCheckable(true);
    _actnShowS->setCheckable(true);
    _actnShowT->setChecked(true);
    _actnShowS->setChecked(true);

    _actnFreeze = toggledAction(tr("Freeze"), this, SLOT(freeze(bool)), ":/toolbar/freeze", Qt::CTRL | Qt::Key_F);

    _actnCalcMediumEnds = toggledAction(tr("Calculate at medium ends"), this, nullptr);
    _actnCalcEmptySpaces = toggledAction(tr("Calculate in empty spaces"), this, nullptr);
    _actnCalcSpaceMids = toggledAction(tr("Calculate in the middle of ranges"), this, nullptr);
    connect(_actnCalcMediumEnds, SIGNAL(triggered(bool)), this, SLOT(toggleCalcMediumEnds(bool)));
    connect(_actnCalcEmptySpaces, SIGNAL(triggered(bool)), this, SLOT(toggleCalcEmptySpaces(bool)));
    connect(_actnCalcSpaceMids, SIGNAL(triggered(bool)), this, SLOT(toggleCalcSpaceMids(bool)));
}

void TableFuncWindow::createMenuBar()
{
    _menuTable = Ori::Gui::menu(tr("Table", "Menu title"), this, {
        _actnUpdate, _actnFreeze, nullptr, _actnShowT, _actnShowS, nullptr,
            Ori::Gui::menu(tr("Options"), this, {_actnCalcMediumEnds, _actnCalcEmptySpaces, _actnCalcSpaceMids})
    });
}

void TableFuncWindow::createToolBar()
{
    _buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");
    auto buttonParams = new QToolButton;
    buttonParams->setMenu(Ori::Gui::menu({_actnCalcMediumEnds, _actnCalcEmptySpaces, _actnCalcSpaceMids}));
    buttonParams->setPopupMode(QToolButton::InstantPopup);
    buttonParams->setIcon(QIcon(":/toolbar/options"));

    _menuColUnits = new QMenu(this);
    connect(_menuColUnits, &QMenu::aboutToShow, this, &TableFuncWindow::updateColUnitsMenu);

    auto buttonUnits = new QToolButton;
    buttonUnits->setPopupMode(QToolButton::InstantPopup);
    buttonUnits->setMenu(_menuColUnits);
    buttonUnits->setIcon(QIcon(":/toolbar/caliper"));

    auto t = toolbar();
    t->addAction(_actnUpdate);
    t->addSeparator();
    t->addAction(_actnFreeze);
    _actnFrozenInfo = t->addWidget(_buttonFrozenInfo);
    t->addSeparator();
    t->addAction(_actnShowT);
    t->addAction(_actnShowS);
    t->addSeparator();
    t->addWidget(buttonParams);
    t->addWidget(buttonUnits);
}

void TableFuncWindow::createStatusBar()
{
    _statusBar = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    setContent(_statusBar);
}

void TableFuncWindow::createContent()
{
    _table = new TableFuncResultTable(_function);
    _table->updateColumnTitles();

    _errorView = new QTextBrowser();
    _errorView->setVisible(false);

    setContent(Ori::Layouts::LayoutV({_table, _errorView}).setMargin(0).setSpacing(0).makeWidget());
}

void TableFuncWindow::update()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }

    _function->calculate();
    if (!_function->ok())
    {
        _errorView->setHtml(QString("<p style='color:red;font-size:13pt;margin:1em;'><br>%1</p>").arg(_function->errorText()));
        _errorView->setVisible(true);
        _table->setVisible(false);
        _table->clearContents();
    }
    else
    {
        _errorView->setVisible(false);
        _errorView->clear();
        _table->setVisible(true);
        updateTable();
    }
}

void TableFuncWindow::activateModeT()
{
    if (!_actnShowT->isChecked() && !_actnShowS->isChecked())
        _actnShowS->setChecked(true);

    updateModeTS();
}

void TableFuncWindow::activateModeS()
{
    if (!_actnShowS->isChecked() && !_actnShowT->isChecked())
        _actnShowT->setChecked(true);

    updateModeTS();
}

void TableFuncWindow::updateModeTS()
{
    showModeTS();
    updateTable();

    schema()->markModified("TableFuncWindow::updateModeTS");
}

void TableFuncWindow::showModeTS()
{
    _table->showT = _actnShowT->isChecked();
    _table->showS = _actnShowS->isChecked();
    _table->updateColumnTitles();
}

void TableFuncWindow::freeze(bool frozen)
{
    _frozen = frozen;
    _actnUpdate->setEnabled(!_frozen);
    _actnFrozenInfo->setVisible(_frozen);
    if (_frozen)
    {
        InfoFuncSummary summary(schema());
        summary.calculate();
        _buttonFrozenInfo->setInfo(summary.result());
    }
    if (!_frozen and _needRecalc)
        update();
}

void TableFuncWindow::updateTable()
{
    _table->updateResults();
}

TableFunction::Params TableFuncWindow::readParams(const QJsonObject& obj)
{
    TableFunction::Params params;
    params.calcMediumEnds = obj["calcMediumEnds"].toBool(false);
    params.calcEmptySpaces = obj["calcEmptySpaces"].toBool(false);
    params.calcSpaceMids = obj["calcSpaceMids"].toBool(false);
    return params;
}

QJsonObject TableFuncWindow::writeParams(const TableFunction::Params& params)
{
    return QJsonObject({
        {"calcMediumEnds", params.calcMediumEnds},
        {"calcEmptySpaces", params.calcEmptySpaces},
        {"calcSpaceMids", params.calcSpaceMids},
    });
}

bool TableFuncWindow::configure()
{
    return configureInternal(readParams(RecentData::getObj("func_beam_params_at_elems")));
}

bool TableFuncWindow::configureInternal(const TableFunction::Params& params)
{
    RecentData::setObj("func_beam_params_at_elems", writeParams(params));
    schema()->events().raise(SchemaEvents::Changed, "TableFuncWindow: configure");
    _function->setParams(params);
    updateParamsActions();
    update();
    return true;
}

void TableFuncWindow::updateParamsActions()
{
    auto params = _function->params();
    _actnCalcMediumEnds->setChecked(params.calcMediumEnds);
    _actnCalcEmptySpaces->setChecked(params.calcEmptySpaces);
    _actnCalcSpaceMids->setChecked(params.calcSpaceMids);
}

void TableFuncWindow::updateColUnitsMenu()
{
    _menuColUnits->clear();
    const auto cols = _function->columns();
    for (int i = 0; i < cols.size(); i++) {
        const auto& col = cols.at(i);
        if (col.unit == Z::Units::none()) continue;
        if (!_unitMenus.contains(i)) {
            auto menu = new UnitsMenu(this);
            connect(menu, &UnitsMenu::unitChanged, this, [i, this](Z::Unit unit){
                const auto col = _function->columns().at(i);
                if (col.unit == unit) return;
                _function->setColumnUnit(col.id, unit);
                _function->schema()->markModified("TableFuncWindow: unit changed");
                _table->updateColumnTitles();
                _table->updateResults();
            });
            _unitMenus.insert(i, menu);
        }
        auto menu = _unitMenus[i];
        menu->setUnit(col.unit);
        auto actn = _menuColUnits->addMenu(menu->menu());
        actn->setText(_function->columnTitle(col.id));
    }
}

void TableFuncWindow::toggleCalcMediumEnds(bool calc)
{
    auto params = _function->params();
    params.calcMediumEnds = calc;
    configureInternal(params);
}

void TableFuncWindow::toggleCalcEmptySpaces(bool calc)
{
    auto params = _function->params();
    params.calcEmptySpaces = calc;
    configureInternal(params);
}

void TableFuncWindow::toggleCalcSpaceMids(bool calc)
{
    auto params = _function->params();
    params.calcSpaceMids = calc;
    configureInternal(params);
}

bool TableFuncWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _function->setParams(readParams(root["function"].toObject()));
    updateParamsActions();
    
    if (auto colsJson = root["columns"].toObject(); !colsJson.isEmpty()) {
        for (auto it = colsJson.constBegin(); it != colsJson.constEnd(); it++) {
            auto colId = it.key();
            auto colJson = it.value().toObject();
            if (colJson.isEmpty()) {
                qWarning() << Q_FUNC_INFO << "Wrong value of key" << it.key() << "Object expected";
                continue;
            }
            auto unitAlias = colJson["unit"].toString();
            auto unit = Z::Units::findByAlias(unitAlias);
            if (!unit) {
                qWarning() << Q_FUNC_INFO << "Unknown unit for column" << it.key() << unitAlias;
                continue;
            }
            _function->setColumnUnit(colId, unit);
        }
    }

    auto modeTS = root["window"].toObject()["ts_mode"].toString();
    bool modeT = true, modeS = true;
    if (modeTS == "T") modeS = false;
    else if (modeTS == "S") modeT = false;
    _actnShowT->setChecked(modeT);
    _actnShowS->setChecked(modeS);
    showModeTS();

    return true;
}

bool TableFuncWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["function"] = writeParams(_function->params());

    if (const auto &colUnits = _function->columntUnits(); !colUnits.isEmpty()) {
        QJsonObject colsJson;
        for (auto it = colUnits.constBegin(); it != colUnits.constEnd(); it++) {
            colsJson[it.key()] = QJsonObject({
                { "unit", it.value()->alias() },
            });
        }
        root["columns"] = colsJson;
    }

    bool modeT = _actnShowT->isChecked();
    bool modeS = _actnShowS->isChecked();
    root["window"] = QJsonObject({
        { "ts_mode", (modeT && modeS)? "T+S": (modeT ? "T" : "S") }
    });

    return true;
}
