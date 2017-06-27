#include "SchemaTable.h"
#include "ElementImagesProvider.h"

#include <QHeaderView>
#include <QItemDelegate>
#include <QMenu>
#include <QPainter>
#include <QVariant>

////////////////////////////////////////////////////////////////////////////////
//                              PixmapDelegate
//
/// Simple delegate class to painting pixmaps in table cells.
////////////////////////////////////////////////////////////////////////////////

class PixmapDelegate : public QItemDelegate
{
public:
    PixmapDelegate(QSize size, QWidget *parent = 0) : QItemDelegate(parent), _iconSize(size)
    {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex& index) const
    {
        if (index.data().canConvert<QPixmap>())
        {
            if (option.state & QStyle::State_Selected)
                painter->fillRect(option.rect, option.palette.highlight());
            painter->drawPixmap(
                QRect(option.rect.left()+2, option.rect.top()+2, _iconSize.width(), _iconSize.height()),
                index.data().value<QPixmap>(), QRect(0, 0, _iconSize.width(), _iconSize.height()));
        }
        else
            QItemDelegate::paint(painter, option, index);
    }

private:
    QSize _iconSize;
};


////////////////////////////////////////////////////////////////////////////////
//                              SchemaTable
////////////////////////////////////////////////////////////////////////////////

SchemaTable::SchemaTable(Schema *schema, QWidget *parent) : QTableWidget(0, 4, parent)
{
    _schema = schema;
    _contextMenu = 0;

    auto iconSize = ElementImagesProvider::instance().iconSize();

    setItemDelegate(new PixmapDelegate(iconSize));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(COL_IMAGE, iconSize.width()+4);
    horizontalHeader()->setSectionResizeMode(COL_LABEL, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_PARAMS, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_TITLE, QHeaderView::Stretch);
#else
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->resizeSection(COL_IMAGE, iconSize.width()+4);
    horizontalHeader()->setResizeMode(COL_LABEL, QHeaderView::ResizeToContents);
    horizontalHeader()->setResizeMode(COL_PARAMS, QHeaderView::ResizeToContents);
    horizontalHeader()->setResizeMode(COL_TITLE, QHeaderView::Stretch);
#endif
    horizontalHeader()->setHighlightSections(false);
    setHorizontalHeaderLabels({ tr("Typ"), tr("Label"), tr("Parameters"), tr("Title") });

    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(doubleClicked(QTableWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

SchemaTable::~SchemaTable()
{
}

void SchemaTable::adjustColumns()
{
    resizeColumnToContents(COL_LABEL);
    resizeColumnToContents(COL_PARAMS);
}

void SchemaTable::doubleClicked(QTableWidgetItem*)
{
    Element* elem = selected();
    if (elem)
        emit doubleClicked(elem);
}

void SchemaTable::showContextMenu(const QPoint& pos)
{
    if (_contextMenu)
        _contextMenu->popup(mapToGlobal(pos));
}

Element* SchemaTable::selected() const
{
    int index = currentRow();
    return (index >= 0 and index < schema()->count())? schema()->element(index): 0;
}

void SchemaTable::setSelected(Element *elem)
{
    setCurrentCell(_schema->indexOf(elem), 0);
}

Elements SchemaTable::selection() const
{
    Elements elements;
    QVector<int> rows = selectedRows();
    for (int i = 0; i < rows.count(); i++)
        elements.append(schema()->element(rows[i]));
    return elements;
}

QVector<int> SchemaTable::selectedRows() const
{
    QVector<int> result;
    QList<QTableWidgetSelectionRange> selection = selectedRanges();
    for (int i = 0; i < selection.count(); i++)
        for (int j = selection.at(i).topRow(); j <= selection.at(i).bottomRow(); j++)
            result.append(j);
    return result;
}

bool SchemaTable::hasSelection() const
{
    return currentRow() > -1;
}

void SchemaTable::populate()
{
    clearContents();
    setRowCount(schema()->count());
    for (int row = 0; row < schema()->count(); row++)
    {
        Element *elem = schema()->element(row);
        createRow(elem, row);
        populateRow(elem, row);
    }
    adjustColumns();
}

void SchemaTable::createRow(Element *elem, int row)
{
    QTableWidgetItem *it = new QTableWidgetItem();
    it->setData(0, QPixmap(ElementImagesProvider::instance().iconPath(elem->type())));
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setTextAlignment(Qt::AlignCenter);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_LABEL, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_PARAMS, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_TITLE, it);
}

void SchemaTable::populateRow(Element *elem, int row)
{
    item(row, COL_LABEL)->setText(elem->label());
    item(row, COL_PARAMS)->setText(elem->params().str());
    item(row, COL_TITLE)->setText(elem->title());
    const QBrush& color = elem->disabled()? palette().shadow() : palette().text();
    item(row, COL_LABEL)->setForeground(color);
    item(row, COL_PARAMS)->setForeground(color);
    item(row, COL_TITLE)->setForeground(color);
}

void SchemaTable::populateParams()
{
    for (int i = 0; i < schema()->count(); i++)
        item(i, COL_PARAMS)->setText(schema()->element(i)->params().str());
}

void SchemaTable::schemaLoaded(Schema*)
{
    populate();
}

void SchemaTable::schemaParamsChanged(Schema*)
{
    // the only interesting event - units of measurements
    // has been changed and element descriptors as well
    populateParams();
    adjustColumns();
}

void SchemaTable::elementCreated(Schema*, Element* elem)
{
    populate();
    setSelected(elem);
}

void SchemaTable::elementChanged(Schema *schema, Element *elem)
{
    int index = schema->indexOf(elem);
    if (index >= 0 && index < rowCount())
    {
        populateRow(elem, index);
        adjustColumns();
    }
}

void SchemaTable::elementDeleting(Schema *schema, Element *elem)
{
    int index = schema->indexOf(elem);
    if (index >= 0 && index < rowCount())
    {
        removeRow(index);
        adjustColumns();
    }
}
