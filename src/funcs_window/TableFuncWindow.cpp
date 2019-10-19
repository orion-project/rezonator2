#include "TableFuncWindow.h"

#include "FuncWindowHelpers.h"
#include "../Appearance.h"
#include "../core/Format.h"
#include "../funcs/InfoFunctions.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/RichTextItemDelegate.h"

#include "helpers/OriWidgets.h"
#include "widgets/OriStatusBar.h"

#include <QAction>
#include <QClipboard>
#include <QHeaderView>
#include <QMenu>
#include <QPainter>
#include <QToolBar>

using namespace Ori::Gui;

struct TableFuncResultPositionInfo
{
    QString ascii;
    QString tooltip;
    QPixmap pixmap;
};

static const TableFuncResultPositionInfo& tableResultPositionInfo(TableFunction::ResultPosition pos)
{
#define I_(pos, ascii, tooltip, pixmap)\
    {TableFunction::ResultPosition::pos, {QString(ascii), QString(tooltip), QPixmap(pixmap)}}

    static QMap<TableFunction::ResultPosition, TableFuncResultPositionInfo> info = {
        I_(ELEMENT,       "",          "",                          ""),
        I_(LEFT,          "->()",      "At the left of element",    ":/misc/beampos_left"),
        I_(RIGHT,         "  ()->",    "At the right of element",   ":/misc/beampos_right"),
        I_(LEFT_OUTSIDE,  "->[   ]",   "At the left edge outside",  ":/misc/beampos_left_out"),
        I_(LEFT_INSIDE,   "  [-> ]",   "At the left edge inside",   ":/misc/beampos_left_in"),
        I_(MIDDLE,        "  [ + ]",   "In the middle of element",  ":/misc/beampos_middle"),
        I_(RIGHT_INSIDE,  "  [ ->]",   "At the right edge inside",  ":/misc/beampos_right_in"),
        I_(RIGHT_OUTSIDE, "  [   ]->", "At the right edge outside", ":/misc/beampos_right_out"),
        I_(IFACE_LEFT,    "->|",       "At the left of interface",  ":/misc/beampos_iface_left"),
        I_(IFACE_RIGHT,   "  |->",     "At the right of interface", ":/misc/beampos_iface_right"),
    };
    return info[pos];
#undef I_
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
    painter->drawPixmap(r.right() - 26, r.top() + 2, tableResultPositionInfo(resultPosition).pixmap);
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
    for (const auto& col : _columns)
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
            it->setData(Qt::UserRole, int(res.position));
            it->setToolTip(tableResultPositionInfo(res.position).tooltip);
            setItem(row, COL_POSITION, it);
        }
        it->setText(res.element->displayLabel());

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
                stream << '\t' << tableResultPositionInfo(resultPosition).ascii;
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
}

void TableFuncWindow::createMenuBar()
{
    _menuTable = menu(tr("Table", "Menu title"), this, {
        _actnUpdate, _actnFreeze, nullptr, _actnShowT, _actnShowS
    });
}

void TableFuncWindow::createToolBar()
{
    _buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");

    auto t = toolbar();
    t->addAction(_actnUpdate);
    t->addSeparator();
    t->addAction(_actnFreeze);
    _actnFrozenInfo = t->addWidget(_buttonFrozenInfo);
    t->addSeparator();
    t->addAction(_actnShowT);
    t->addAction(_actnShowS);
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

    setContent(_table);
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
        showStatusError(_function->errorText());
        _table->clearContents();
    }
    else
    {
        clearStatusInfo();
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

void TableFuncWindow::showStatusError(const QString& message)
{
    _statusBar->setText(STATUS_INFO, message);
    _statusBar->highlightError(STATUS_INFO);
    _statusBar->setVisible(true);
}

void TableFuncWindow::clearStatusInfo()
{
    _statusBar->clear(STATUS_INFO);
    _statusBar->setVisible(false);
}

