#include "ElementsTable.h"

#include "RichTextItemDelegate.h"
#include "../Appearance.h"
#include "../funcs/FormatInfo.h"

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QMenu>

namespace {

QPixmap elemIcon(const QString& type)
{
    static QMap<QString, QPixmap> icons;
    if (!icons.contains(type))
        icons[type] = QPixmap(Z::Utils::elemIconPath(type));
    return icons[type];
}

enum { COL_IMAGE, COL_LABEL, COL_PARAMS, COL_TITLE, COL_COUNT };

} // namespace

//------------------------------------------------------------------------------
//                             ElementsTableModes
//------------------------------------------------------------------------------

class ElementsTableModel : public QAbstractTableModel, public SchemaListener
{
public:
    ElementsTableModel(Schema* schema, QTableView* view): QAbstractTableModel(nullptr), _schema(schema), _view(view)
    {
    }

    int rowCount(const QModelIndex&) const override { return _schema->count()+1; }
    int columnCount(const QModelIndex&) const override { return COL_COUNT; }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (role == Qt::DisplayRole)
        {
            switch (orientation)
            {
            case Qt::Vertical:
                return section + 1;
            case Qt::Horizontal:
                switch (section)
                {
                case COL_IMAGE: return tr("Typ");
                case COL_LABEL: return tr("Label");
                case COL_PARAMS: return tr("Parameters");
                case COL_TITLE: return tr("Title");
                }
            }
        }
        return QVariant();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid()) return QVariant();
        if (role == Qt::DecorationRole)
        {
            if (index.column() == COL_IMAGE)
            {
                auto elem = _schema->element(index.row());
                return elem ? elemIcon(elem->type()) : _addElemIcon;
            }
        }
        else if (role == Qt::ToolTipRole)
        {
            if (index.column() == COL_IMAGE)
            {
                auto elem = _schema->element(index.row());
                return elem ? elem->typeName() : QVariant();
            }
        }
        else if (role == Qt::FontRole)
        {
            if (index.column() == COL_LABEL)
                return Z::Gui::ElemLabelFont().get();
        }
        else if (role == Qt::TextAlignmentRole)
        {
            if (index.column() == COL_LABEL)
                return Qt::AlignCenter;
        }
        else if (role == Qt::ForegroundRole)
        {
            // it's the "double click to add" row
            if (index.row() == _schema->count())
                return QColor(0, 0, 0, 40);
        }
        else if (role == Qt::DisplayRole)
        {
            auto elem = _schema->element(index.row());
            if (elem)
            {
                switch (index.column()) {
                case COL_LABEL: return elem->label();
                case COL_TITLE: return elem->title();
                case COL_PARAMS:
                {
                    Z::Format::FormatElemParams f;
                    f.schema = _schema;
                    return f.format(elem);
                }
                }
            }
            else if (index.column() == COL_TITLE)
                return tr("Double click here to append a new element");
        }
        return QVariant();
    }

    void schemaLoaded(Schema*) override
    {
        emit layoutChanged();
    }

    void schemaRebuilt(Schema*) override
    {
        emit dataChanged(index(0, 0), index(_schema->count(), COL_COUNT));
    }

    void elementChanged(Schema*, Element* elem) override
    {
        int row = _schema->indexOf(elem);
        emit dataChanged(index(row, 0), index(row, COL_COUNT));
        adjustColumns();
    }

    void elementCreated(Schema*, Element* elem) override
    {
        int row = _schema->indexOf(elem);
        beginInsertRows(QModelIndex(), row, row);
        endInsertRows();
        adjustColumns();
    }

    void elementsDeleting(Schema*) override
    {
        _deleted.clear();
    }

    void elementDeleting(Schema*, Element* elem) override
    {
        _deleted << _schema->indexOf(elem);
    }

    void elementsDeleted(Schema*) override
    {
        int minRow = _schema->count()-1, maxRow = 0;
        foreach (int row, _deleted) {
            if (row < minRow) minRow = row;
            if (row > maxRow) maxRow = row;
        }
        beginRemoveRows(QModelIndex(), minRow, maxRow);
        endRemoveRows();
        adjustColumns();
        _deleted.clear();
    }

    void adjustColumns()
    {
        _view->resizeColumnToContents(COL_LABEL);
        _view->resizeColumnToContents(COL_PARAMS);
    }

private:
    Schema* _schema;
    QTableView* _view;
    QSet<int> _deleted;
    QPixmap _addElemIcon = QPixmap(":/toolbar/elem_add");
};

//------------------------------------------------------------------------------
//                               ElementsTable
//------------------------------------------------------------------------------

ElementsTable::ElementsTable(Schema *schema, QWidget *parent) : QTableView(parent), _schema(schema)
{
    _model = new ElementsTableModel(schema, this);
    _schema->registerListener(_model);

    setModel(_model);
    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &ElementsTable::currentRowChanged);

    auto iconSize = Z::Utils::elemIconSize();

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
    auto h = horizontalHeader();
    h->setMinimumSectionSize(iconSize.width()+6);
    h->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    h->resizeSection(COL_IMAGE, iconSize.width()+6);
    h->setSectionResizeMode(COL_LABEL, QHeaderView::ResizeToContents);
    h->setSectionResizeMode(COL_PARAMS, QHeaderView::ResizeToContents);
    h->setSectionResizeMode(COL_TITLE, QHeaderView::Stretch);
    h->setHighlightSections(false);

    connect(this, &ElementsTable::doubleClicked, this, &ElementsTable::indexDoubleClicked);
    connect(this, &ElementsTable::customContextMenuRequested, this, &ElementsTable::showContextMenu);
}

ElementsTable::~ElementsTable()
{
    _schema->unregisterListener(_model);
    delete _model;
}

Elements ElementsTable::selection() const
{
    Elements elems;
    foreach (const auto& index, selectionModel()->selectedRows()) {
        auto elem = _schema->element(index.row());
        if (elem) elems << elem;
    }
    return elems;
}

void ElementsTable::selectElems(const Elements& elems)
{
   int minRow = _schema->count()-1, maxRow = 0;
   foreach (auto elem, elems) {
       int row = _schema->indexOf(elem);
       if (row < minRow) minRow = row;
       if (row > maxRow) maxRow = row;
   }
   selectRow(maxRow); // update current index
   selectionModel()->select(
        QItemSelection(_model->index(minRow, 0), _model->index(maxRow, COL_COUNT-1)),
        QItemSelectionModel::SelectionFlags(QItemSelectionModel::Clear | QItemSelectionModel::Select));
}

int ElementsTable::currentRow() const
{
    return selectionModel()->currentIndex().row();
}

Element* ElementsTable::currentElem() const
{
    return _schema->element(currentRow());
}

void ElementsTable::setCurrentElem(Element* elem)
{
    // selectRows also also sets currentIndex in selection model, so they stay in sync
    selectRow(_schema->indexOf(elem));
}

void ElementsTable::currentRowChanged(const QModelIndex &current, const QModelIndex&)
{
    emit currentElemChanged(_schema->element(current.row()));
}

void ElementsTable::indexDoubleClicked(const QModelIndex &index)
{
    emit elemDoubleClicked(_schema->element(index.row()));
}

void ElementsTable::showContextMenu(const QPoint& pos)
{
    auto menu = (currentRow() < _schema->count() - 1) ? elementContextMenu : lastRowContextMenu;
    if (!menu) return;
    emit beforeContextMenuShown(menu);
    menu->popup(mapToGlobal(pos));
}
