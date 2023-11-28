#include "TableFuncWindow.h"

#include "../app/Appearance.h"
#include "../app/CustomPrefs.h"
#include "../core/Format.h"
#include "../funcs/FuncWindowHelpers.h"
#include "../math/InfoFunctions.h"
#include "../widgets/FrozenStateButton.h"

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

using namespace Ori::Gui;

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

TableFuncResultTable::TableFuncResultTable(const QVector<TableFunction::ColumnDef> &columns) : QTableWidget(), _columns(columns)
{
    setWordWrap(false);
    setColumnCount(FIXED_COLS_COUNT + _columns.size());
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setItemDelegateForColumn(COL_POSITION, new TableFuncPositionColumnItemDelegate(this));

    connect(this, &QTableWidget::customContextMenuRequested, this, &TableFuncResultTable::showContextMenu);
}

void TableFuncResultTable::updateColumnTitles()
{
    QStringList titles;
    titles << tr("Position");
    for (auto& col : _columns)
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

void TableFuncResultTable::update(const QVector<TableFunction::Result>& results)
{
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

        for (int index = 0; index < res.values.size(); index++)
        {
            const auto& column = _columns.at(index);
            const auto& value = res.values.at(index);
            double valueT = column.unit->fromSi(value.T);
            double valueS = column.unit->fromSi(value.S);

            QString valueStr;
            if (showT and showS)
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

void TableFuncResultTable::showContextMenu(const QPoint& pos)
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

    _actnCalcMediumEnds = toggledAction(tr("Calculate at medium ends"), this, SLOT(toggleCalcMediumEnds(bool)));
    _actnCalcEmptySpaces = toggledAction(tr("Calculate in empty spaces"), this, SLOT(toggleCalcEmptySpaces(bool)));
    _actnCalcSpaceMids = toggledAction(tr("Calculate in the middle of ranges"), this, SLOT(toggleCalcSpaceMids(bool)));
}

void TableFuncWindow::createMenuBar()
{
    _menuTable = menu(tr("Table", "Menu title"), this, {
        _actnUpdate, _actnFreeze, nullptr, _actnShowT, _actnShowS, nullptr,
            menu(tr("Options"), this, {_actnCalcMediumEnds, _actnCalcEmptySpaces, _actnCalcSpaceMids})
    });
}

void TableFuncWindow::createToolBar()
{
    _buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");
    auto buttonParams = new QToolButton;
    buttonParams->setMenu(menu({_actnCalcMediumEnds, _actnCalcEmptySpaces, _actnCalcSpaceMids}));
    buttonParams->setPopupMode(QToolButton::InstantPopup);
    buttonParams->setIcon(QIcon(":/toolbar/options"));

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
}

void TableFuncWindow::createStatusBar()
{
    _statusBar = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    setContent(_statusBar);
}

void TableFuncWindow::createContent()
{
    _table = new TableFuncResultTable(_function->columns());
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
    _table->update(_function->results());
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
    return configureInternal(readParams(CustomPrefs::recentObj("func_beam_params_at_elems")));
}

bool TableFuncWindow::configureInternal(const TableFunction::Params& params)
{
    CustomPrefs::setRecentObj("func_beam_params_at_elems", writeParams(params));
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

    bool modeT = _actnShowT->isChecked();
    bool modeS = _actnShowS->isChecked();
    root["window"] = QJsonObject({
        { "ts_mode", (modeT && modeS)? "T+S": (modeT ? "T" : "S") }
    });

    return true;
}
