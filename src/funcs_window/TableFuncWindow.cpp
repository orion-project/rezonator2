#include "TableFuncWindow.h"

#include "FuncWindowHelpers.h"
#include "../core/Format.h"
#include "../funcs/InfoFunctions.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/RichTextItemDelegate.h"

#include "helpers/OriWidgets.h"
#include "widgets/OriStatusBar.h"

#include <QAction>
#include <QHeaderView>
#include <QToolBar>

using namespace Ori::Gui;

enum PlotWindowStatusPanels
{
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

//------------------------------------------------------------------------------
//                             TableFuncResultTable
//------------------------------------------------------------------------------

TableFuncResultTable::TableFuncResultTable(const QVector<TableFunction::ColumnDef> &columns) : QTableWidget(), _columns(columns)
{
    setWordWrap(false);
    setColumnCount(_columns.size());
    setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int col = 0; col < columns.size(); col++)
    {
        const auto& column = columns.at(col);
        if (column.isHtml)
            setItemDelegateForColumn(col, new RichTextItemDelegate(0, this));
        if (column.width == TableFunction::ColumnDef::WIDTH_STRETCH)
            horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
        else if (column.width == TableFunction::ColumnDef::WIDTH_AUTO)
            horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
        else
        {
            horizontalHeader()->setSectionResizeMode(col, QHeaderView::Fixed);
            horizontalHeader()->resizeSection(col, column.width);
        }
    }
}

void TableFuncResultTable::adjustColumns()
{
    for (int col = 0; col < columnCount(); col++)
        if (horizontalHeader()->sectionResizeMode(col) == QHeaderView::ResizeToContents)
            resizeColumnToContents(col);
}

void TableFuncResultTable::updateColumnTitles(bool t, bool s)
{
    QStringList titles;
    for (const auto& col : _columns)
    {
        QString title;
        if (!col.title.isEmpty())
            title = col.title;
        else if (t and s)
            title = QString("%1 %2 %3 ").arg(col.titleT).arg(Z::Strs::multX()).arg(col.titleS);
        else if (t)
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

//------------------------------------------------------------------------------
//                                TableFuncWindow
//------------------------------------------------------------------------------

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
    _table->updateColumnTitles(true, true);

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
        _table->clear();
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
    _table->updateColumnTitles(_actnShowT->isChecked(), _actnShowS->isChecked());
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
    // TODO
    _table->adjustColumns();
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

