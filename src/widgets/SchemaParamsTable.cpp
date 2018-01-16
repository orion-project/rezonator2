#include "SchemaParamsTable.h"
#include "ElementImagesProvider.h"
#include "PixmapItemDelegate.h"

#include <QHeaderView>

SchemaParamsTable::SchemaParamsTable(Schema *schema, QWidget *parent) : QTableWidget(0, COL_COUNT, parent)
{
    _schema = schema;

    auto iconSize = ElementImagesProvider::instance().iconSize();

    setItemDelegate(new PixmapDelegate(iconSize));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->setMinimumSectionSize(iconSize.width()+4);
    horizontalHeader()->resizeSection(COL_IMAGE, iconSize.width()+4);
    horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_TITLE, QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);
    setHorizontalHeaderLabels({ tr("Typ"), tr("Name"), tr("Value"), tr("Comment") });
}

void SchemaParamsTable::adjustColumns()
{
    resizeColumnToContents(COL_NAME);
    resizeColumnToContents(COL_VALUE);
}
