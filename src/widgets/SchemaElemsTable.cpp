#include "SchemaElemsTable.h"

#include "ElementImagesProvider.h"
#include "RichTextItemDelegate.h"
#include "../Appearance.h"
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
    if (!menu) return;
    emit beforeContextMenuShown(menu);
    menu->popup(mapToGlobal(pos));
}

Element* SchemaElemsTable::selected() const
{
    return schema()->element(currentRow());
}

void SchemaElemsTable::setSelected(Element *elem)
{
    setCurrentCell(_schema->indexOf(elem), 0);
}

Elements SchemaElemsTable::selection() const
{
    Elements elements;
    foreach (int row, selectedRows())
        elements << schema()->element(row);
    return elements;
}

QList<int> SchemaElemsTable::selectedRows() const
{
    QList<int> rows;
    foreach (auto index, selectionModel()->selectedIndexes())
    {
        // Don't include the last row because it's the "Create element" placeholder
        if (index.row() == rowCount() - 1) continue;
        if (!rows.contains(index.row())) rows << index.row();
    }
    std::sort(rows.begin(), rows.end());
    return rows;
}

// TODO: This method should be optimized, it takes 0.1 - 0.2s for schema of 7 elements.
// For comparison, SchemaLayout::populate() takes about a couple of ms on the same schema.
// When click "Move Up" or "Move Down" one can see the table really hangs for a little while.
// Maybe it worth to refuse using QTableWidget and switch to QTableView with custom model.
// Event single call of populateRow()/adjustColumns() takes about 10ms.
void SchemaElemsTable::populate()
{
    clearContents();
    const auto& elems = schema()->elements();
    setRowCount(elems.size() + 1);
    for (int row = 0; row < elems.size(); row++)
    {
        Element *elem = elems.at(row);
        createRow(elem, row);
        populateRow(elem, row);
    }
    fillPlaceholderRow();
    adjustColumns();
}

namespace {

QPixmap elemIcon(const QString& type)
{
    static QMap<QString, QPixmap> icons;
    if (!icons.contains(type))
        icons[type] = QPixmap(Z::Utils::elemIconPath(type));
    return icons[type];
}

} // namespace

void SchemaElemsTable::createRow(Element *elem, int row)
{
    QTableWidgetItem *it = new QTableWidgetItem();
    it->setData(Qt::UserRole, QVariant::fromValue(elem));
    it->setData(Qt::DecorationRole, elemIcon(elem->type()));
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setToolTip(elem->typeName());
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setFont(Z::Gui::ElemLabelFont().get());
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
    item(row, COL_LABEL)->setText(elem->label());

    Z::Format::FormatElemParams f;
    f.schema = schema();
    item(row, COL_PARAMS)->setText(f.format(elem));

    item(row, COL_TITLE)->setText(elem->title());
    const QBrush& color = elem->disabled()? palette().shadow() : palette().text();
    item(row, COL_LABEL)->setForeground(color);
    item(row, COL_PARAMS)->setForeground(color);
    item(row, COL_TITLE)->setForeground(color);
}

void SchemaElemsTable::fillPlaceholderRow()
{
    int row = rowCount() - 1;

    QTableWidgetItem *it = new QTableWidgetItem();
    it->setData(Qt::DecorationRole, QPixmap(":/toolbar/elem_add"));
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_LABEL, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_PARAMS, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setForeground(QColor(0, 0, 0, 40));
    it->setText(tr("Double click here to append a new element"));
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
    Q_UNUSED(schema);
    // Don't use schema.indexOf for getting element raw number,
    // It gives wrong row numbers when deleting several elements.
    for (int row = 0; row < rowCount(); row++)
    {
        auto data = item(row, COL_IMAGE)->data(Qt::UserRole);
        auto rowElem = data.value<Element*>();
        if (rowElem == elem)
        {
            removeRow(row);
            adjustColumns();
            return;
        }
    }
}
