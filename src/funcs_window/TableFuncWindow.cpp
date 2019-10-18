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
#include <QHeaderView>
#include <QPainter>
#include <QToolBar>

using namespace Ori::Gui;

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

    painter->drawPixmap(r.right() - 26, r.top() + 2, qvariant_cast<QPixmap>(_paintingIndex.data(Qt::UserRole)));
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
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setItemDelegateForColumn(COL_POSITION, new TableFuncPositionColumnItemDelegate(this));
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

    static QMap<TableFunction::ResultPosition, QPixmap> pixmaps {
        {TableFunction::ResultPosition::ELEMENT, QPixmap()},
        {TableFunction::ResultPosition::LEFT, QPixmap(":/misc/beampos_left")},
        {TableFunction::ResultPosition::RIGHT, QPixmap(":/misc/beampos_right")},
        {TableFunction::ResultPosition::LEFT_INSIDE, QPixmap(":/misc/beampos_left_in")},
        {TableFunction::ResultPosition::LEFT_OUTSIDE, QPixmap(":/misc/beampos_left_out")},
        {TableFunction::ResultPosition::MIDDLE, QPixmap(":/misc/beampos_middle")},
        {TableFunction::ResultPosition::RIGHT_INSIDE, QPixmap(":/misc/beampos_right_in")},
        {TableFunction::ResultPosition::RIGHT_OUTSIDE, QPixmap(":/misc/beampos_right_out")},
    };

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
            it->setData(Qt::UserRole, pixmaps[res.position]);
            setItem(row, COL_POSITION, it);
        }
        it->setText(res.element->displayLabel());

        for (int index = 0; index < res.values.size(); index++)
        {
            const auto& value = res.values.at(index);
            // TODO: rescale value to target unit

            QString valueStr;
            if (showT and showS)
                valueStr = QStringLiteral("%1 %2 %3 ")
                        .arg(Z::format(value.T))
                        .arg(Z::Strs::multX())
                        .arg(Z::format(value.S));
            else if (showT)
                valueStr = Z::format(value.T);
            else
                valueStr = Z::format(value.S);

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

