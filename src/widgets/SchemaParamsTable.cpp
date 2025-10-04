#include "SchemaParamsTable.h"

#include "RichTextItemDelegate.h"
#include "../app/Appearance.h"
#include "../core/Perf.h"
#include "../math/FormatInfo.h"

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QMenu>

namespace {

enum { COL_IMAGE, COL_ALIAS, COL_VALUE, COL_ANNOTATION, COL_COUNT };

} // namespace

//------------------------------------------------------------------------------
//                             SchemaParamsTableModel
//------------------------------------------------------------------------------

class SchemaParamsTableModel : public QAbstractTableModel, public SchemaListener, public Z::ParameterListener
{
public:
    SchemaParamsTableModel(Schema* schema, QTableView* view): QAbstractTableModel(nullptr), _schema(schema), _view(view)
    {
        _schema->registerListener(this);
    }
    
    ~SchemaParamsTableModel()
    {
        for (const auto p: *(_schema->globalParams()))
            p->removeListener(this);
        _schema->unregisterListener(this);
    }

    int rowCount(const QModelIndex&) const override { return _schema->globalParams()->size(); }
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
                case COL_ALIAS: return tr("Name");
                case COL_VALUE: return tr("Value");
                case COL_ANNOTATION: return tr("Description");
                }
            }
        }
        return QVariant();
    }
    
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!index.isValid()) return QVariant();
        int col = index.column();
        int row = index.row();
        auto param = _schema->globalParams()->byIndex(row);
        if (!param)
            return QVariant();
        if (role == Qt::DecorationRole)
        {
            if (col == COL_IMAGE)
            {
                if (param->failed())
                    return _iconError;
                if (param->valueDriver() == Z::ParamValueDriver::Formula)
                    return _iconFormula;
                return _iconParam;
            }
        }
        else if (role == Qt::ToolTipRole)
        {
            if (col == COL_IMAGE)
            {
                if (param->failed())
                    return param->error();
                if (param->valueDriver() == Z::ParamValueDriver::Formula)
                    return tr("Formula driven parameter");
                return tr("Simple parameter");
            }
        }
        else if (role == Qt::FontRole)
        {
            if (col == COL_VALUE)
            {
                return Z::Gui::ValueFont().readOnly(param->valueDriver() == Z::ParamValueDriver::Formula).get();
            }
        }
        else if (role == Qt::TextAlignmentRole)
        {
            if (col == COL_VALUE)
                return Qt::AlignCenter;
        }
        else if (role == Qt::DisplayRole)
        {
            switch (col) {
            case COL_ALIAS: {
                Z::Format::FormatParam f;
                f.schema = _schema;
                f.isElement = false;
                return f.format(param);
            }
            case COL_VALUE: return param->value().displayStr();
            case COL_ANNOTATION: return param->description();
            }
        }
        return QVariant();
    }
    
    void schemaLoaded(Schema*) override
    {
        emit layoutChanged();

        for (const auto p: *(_schema->globalParams()))
            p->addListener(this);
    }
    
    void globalParamCreated(Schema*, Z::Parameter* param) override
    {
        int row = findRow(param);
        beginInsertRows(QModelIndex(), row, row);
        endInsertRows();
        adjustColumns();
        param->addListener(this);
    }
    
    void globalParamDeleting(Schema*, Z::Parameter*) override
    {
        beginResetModel();
    }
    
    void globalParamDeleted(Schema*, Z::Parameter*) override
    {
        endResetModel();
        adjustColumns();
    }
    
    void globalParamEdited(Schema*, Z::Parameter* param) override
    {
        updateParamRow(param);
    }
    
    void parameterChanged(Z::ParameterBase* param) override
    {
        updateParamRow(param);
    }
    
    void parameterFailed(Z::ParameterBase* param) override
    {
        updateParamRow(param);
    }
    
private:
    Schema* _schema;
    QTableView* _view;
    QPixmap _iconParam = QIcon(":/toolbar/param_manual").pixmap(Z::Utils::elemIconSize());
    QPixmap _iconFormula = QIcon(":/toolbar/param_formula").pixmap(Z::Utils::elemIconSize());
    QPixmap _iconError = QIcon(":/toolbar/param_warn").pixmap(Z::Utils::elemIconSize());

    void updateParamRow(Z::ParameterBase* param)
    {
        Z_PERF_BEGIN("SchemaParamsTableModel::updateParamRow")

        int row = findRow(param);
        emit dataChanged(index(row, 0), index(row, COL_COUNT-1));
        adjustColumns();
    
        Z_PERF_END
    }
    
    int findRow(Z::ParameterBase* param) const
    {
        return _schema->globalParams()->indexOf((Z::Parameter*)param);
    }

    void adjustColumns()
    {
        _view->resizeColumnToContents(COL_ALIAS);
        _view->resizeColumnToContents(COL_VALUE);
    }
};

//------------------------------------------------------------------------------
//                             SchemaParamsTable
//------------------------------------------------------------------------------

SchemaParamsTable::SchemaParamsTable(Schema *schema, QWidget *parent) : QTableView(parent), _schema(schema)
{
    int aliasOffsetY = 0;
#if defined(Q_OS_MAC)
    aliasOffsetY = 2;
#elif defined(Q_OS_LINUX)
    aliasOffsetY = 1;
#elif defined(Q_OS_WIN)
    aliasOffsetY = 2;
#endif
    
    _model = new SchemaParamsTableModel(schema, this);
    
    setModel(_model);
    
    auto iconSize = Z::Utils::elemIconSize();
    
    setWordWrap(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegateForColumn(COL_ALIAS, new RichTextItemDelegate(aliasOffsetY, this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->setMinimumSectionSize(iconSize.width()+6);
    horizontalHeader()->resizeSection(COL_IMAGE, iconSize.width()+6);
    horizontalHeader()->setSectionResizeMode(COL_ALIAS, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_ANNOTATION, QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);

    connect(this, &SchemaParamsTable::doubleClicked, this, &SchemaParamsTable::indexDoubleClicked);
    connect(this, &SchemaParamsTable::customContextMenuRequested, this, &SchemaParamsTable::showContextMenu);
}

SchemaParamsTable::~SchemaParamsTable()
{
    delete _model;
}

int SchemaParamsTable::currentRow() const
{
    return selectionModel()->currentIndex().row();
}

void SchemaParamsTable::indexDoubleClicked(const QModelIndex &index)
{
    Z::Parameter* param = _schema->globalParams()->byIndex(index.row());
    if (param)
        emit paramDoubleClicked(param);
}

void SchemaParamsTable::showContextMenu(const QPoint& pos)
{
    if (_contextMenu)
        _contextMenu->popup(mapToGlobal(pos));
}

Z::Parameter* SchemaParamsTable::selected() const
{
    return schema()->globalParams()->byIndex(currentRow());
}

void SchemaParamsTable::setSelected(Z::Parameter *param)
{
    selectRow(_schema->globalParams()->indexOf(param));
}
