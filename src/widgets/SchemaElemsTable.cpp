#include "SchemaElemsTable.h"

#include "Appearance.h"
#include "ElementImagesProvider.h"
#include "RichTextItemDelegate.h"
#include "../funcs/FormatInfo.h"

#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QVariant>

SchemaElemsTable::SchemaElemsTable(Schema *schema, QWidget *parent) : QTableWidget(0, COL_COUNT, parent)
{
    _schema = schema;

    auto iconSize = ElementImagesProvider::instance().iconSize();

    int paramsOffsetY = 0;
#if defined(Q_OS_MAC)
    paramsOffsetY = 2;
#elif defined(Q_OS_LINUX)
    paramsOffsetY = 1;
#elif defined(Q_OS_WIN)
    paramsOffsetY = 2;
#endif

    setWordWrap(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegateForColumn(COL_PARAMS, new RichTextItemDelegate(paramsOffsetY, this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->setMinimumSectionSize(iconSize.width()+6);
    horizontalHeader()->resizeSection(COL_IMAGE, iconSize.width()+6);
    horizontalHeader()->setSectionResizeMode(COL_LABEL, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_PARAMS, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_TITLE, QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);
    setHorizontalHeaderLabels({ tr("Typ"), tr("Label"), tr("Parameters"), tr("Title") });

    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(doubleClicked(QTableWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    setRowCount(1);
    fillPlaceholderRow();
}

void SchemaElemsTable::adjustColumns()
{
    resizeColumnToContents(COL_LABEL);
    resizeColumnToContents(COL_PARAMS);
}

void SchemaElemsTable::doubleClicked(QTableWidgetItem*)
{
    emit doubleClicked(selected());
}

void SchemaElemsTable::showContextMenu(const QPoint& pos)
{
    auto menu = (currentRow() < rowCount() - 1) ? elementContextMenu : lastRowContextMenu;
    if (menu) menu->popup(mapToGlobal(pos));
}

Element* SchemaElemsTable::selected() const
{
    int index = currentRow();
    return (index >= 0 and index < schema()->count())? schema()->element(index): nullptr;
}

void SchemaElemsTable::setSelected(Element *elem)
{
    setCurrentCell(_schema->indexOf(elem), 0);
}

Elements SchemaElemsTable::selection() const
{
    Elements elements;
    QList<int> rows = selectedRows();
    for (int i = 0; i < rows.count(); i++)
        elements.append(schema()->element(rows[i]));
    if (elements.empty())
    {
        auto elem = selected();
        if (elem) elements << elem;
    }
    return elements;
}

QList<int> SchemaElemsTable::selectedRows() const
{
    QList<int> result;
    QList<QTableWidgetSelectionRange> selection = selectedRanges();
    for (int i = 0; i < selection.count(); i++)
        for (int j = selection.at(i).topRow(); j <= selection.at(i).bottomRow(); j++)
            if (j < rowCount()-1)
                result.append(j);
    return result;
}

bool SchemaElemsTable::hasSelection() const
{
    int row = currentRow();
    return row > -1 && row < rowCount()-1;
}

// TODO: This method should be optimized, it takes 0.1 - 0.2s for schema of 7 elements.
// For comparison, SchemaLayout::populate() takes about a couple of ms on the same schema.
// When click "Move Up" or "Move Down" one can see the table really hangs for a little while.
// Maybe it worth to refuse using QTableWidget and switch to QTableView with custom model.
// Event single call of populateRow()/adjustColumns() takes about 10ms.
void SchemaElemsTable::populate()
{
    clearContents();
    setRowCount(schema()->count() + 1);
    for (int row = 0; row < schema()->count(); row++)
    {
        Element *elem = schema()->element(row);
        createRow(elem, row);
        populateRow(elem, row);
    }
    fillPlaceholderRow();
    adjustColumns();
}

void SchemaElemsTable::createRow(Element *elem, int row)
{
    QTableWidgetItem *it = new QTableWidgetItem();
    it->setData(Qt::DecorationRole, QPixmap(ElementImagesProvider::instance().iconPath(elem->type())));
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setToolTip(elem->typeName());
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    Z::Gui::setSymbolFont(it);
    it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_LABEL, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_PARAMS, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_TITLE, it);
}

void SchemaElemsTable::populateRow(Element *elem, int row)
{
    item(row, COL_LABEL)->setText(" " % elem->label() % " ");
    item(row, COL_PARAMS)->setText(Z::Format::elemParamsWithValues(schema(), elem));
    item(row, COL_TITLE)->setText("  " % elem->title());
    const QBrush& color = elem->disabled()? palette().shadow() : palette().text();
    item(row, COL_LABEL)->setForeground(color);
    item(row, COL_PARAMS)->setForeground(color);
    item(row, COL_TITLE)->setForeground(color);
}

void SchemaElemsTable::fillPlaceholderRow()
{
    int row = rowCount() - 1;

    QTableWidgetItem *it = new QTableWidgetItem();
    //it->setData(Qt::DecorationRole, QPixmap(":/toolbar/elem_add"));
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_LABEL, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_PARAMS, it);

    it = new QTableWidgetItem();
    Z::Gui::setFontStyle(it, false, true);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setForeground(QColor(0, 0, 0, 30));
    it->setText(tr("Double click here to append new element"));
    setItem(row, COL_TITLE, it);
}

void SchemaElemsTable::schemaLoaded(Schema*)
{
    populate();
}

void SchemaElemsTable::schemaRebuilt(Schema*)
{
    populate();
}

void SchemaElemsTable::elementCreated(Schema*, Element* elem)
{
    populate();
    setSelected(elem);
}

void SchemaElemsTable::elementChanged(Schema *schema, Element *elem)
{
    int index = schema->indexOf(elem);
    if (index >= 0 && index < rowCount()-1)
    {
        populateRow(elem, index);
        adjustColumns();
    }
}

void SchemaElemsTable::elementDeleting(Schema *schema, Element *elem)
{
    int index = schema->indexOf(elem);
    if (index >= 0 && index < rowCount()-1)
    {
        removeRow(index);
        adjustColumns();
    }
}

