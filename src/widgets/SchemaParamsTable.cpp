#include "SchemaParamsTable.h"

#include "RichTextItemDelegate.h"
#include "../app/Appearance.h"
#include "../core/Perf.h"
#include "../math/FormatInfo.h"

#include <QHeaderView>
#include <QMenu>

SchemaParamsTable::SchemaParamsTable(Schema *schema, QWidget *parent) : QTableWidget(0, COL_COUNT, parent), _schema(schema)
{
    int aliasOffsetY = 0;
#if defined(Q_OS_MAC)
    aliasOffsetY = -1;
#endif
    // TODO check windows

    setWordWrap(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegateForColumn(COL_ALIAS, new RichTextItemDelegate(aliasOffsetY, this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->setMinimumSectionSize(_iconSize+6);
    horizontalHeader()->resizeSection(COL_IMAGE, _iconSize+6);
    horizontalHeader()->setSectionResizeMode(COL_ALIAS, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_VALUE, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(COL_ANNOTATION, QHeaderView::Stretch);
    horizontalHeader()->setHighlightSections(false);
    setHorizontalHeaderLabels({ tr("Typ"), tr("Name"), tr("Value"), tr("Annotation") });

    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(doubleClicked(QTableWidgetItem*)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    populate();
}

SchemaParamsTable::~SchemaParamsTable()
{
    for (auto p: *(schema()->customParams()))
        p->removeListener(this);
}

void SchemaParamsTable::adjustColumns()
{
    resizeColumnToContents(COL_ALIAS);
    resizeColumnToContents(COL_VALUE);
}

void SchemaParamsTable::doubleClicked(QTableWidgetItem*)
{
    Z::Parameter* param = selected();
    if (param)
        emit doubleClicked(param);
}

void SchemaParamsTable::showContextMenu(const QPoint& pos)
{
    if (_contextMenu)
        _contextMenu->popup(mapToGlobal(pos));
}

Z::Parameter* SchemaParamsTable::selected() const
{
    return schema()->customParams()->byIndex(currentRow());
}

void SchemaParamsTable::setSelected(Z::Parameter *param)
{
    setCurrentCell(_schema->customParams()->indexOf(param), 0);
}

void SchemaParamsTable::populate()
{
    clearContents();
    setRowCount(schema()->customParams()->size());
    for (int row = 0; row < schema()->customParams()->size(); row++)
    {
        auto param = schema()->customParams()->byIndex(row);
        param->addListener(this);
        createRow(row);
        populateRow(param, row);
    }
    adjustColumns();
}

void SchemaParamsTable::createRow(int row)
{
    QTableWidgetItem *it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_IMAGE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    setItem(row, COL_ALIAS, it);

    it = new QTableWidgetItem();
    it->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_VALUE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_ANNOTATION, it);
}

void SchemaParamsTable::populateRow(Z::Parameter *param, int row)
{
    // Parameter alias and formula
    Z::Format::FormatParam f;
    f.schema = schema();
    f.isElement = false;
    item(row, COL_ALIAS)->setText(f.format(param));

    // Parameter icon
    auto it = item(row, COL_IMAGE);
    QString iconPath, toolTip;
    if (param->valueDriver() == Z::ParamValueDriver::Formula)
    {
        iconPath = ":/toolbar/param_formula";
        toolTip = tr("Formula driven parameter");
    }
    else
    {
        iconPath = ":/toolbar/parameter";
        toolTip = tr("Simple parameter");
    }
    it->setData(Qt::DecorationRole, QIcon(iconPath).pixmap(_iconSize, _iconSize));
    it->setToolTip(toolTip);

    // Parameter value
    it = item(row, COL_VALUE);
    it->setFont(Z::Gui::ValueFont().readOnly(param->valueDriver() == Z::ParamValueDriver::Formula).get());
    it->setText(" " % param->value().displayStr() % " ");

    // Parameter annotation
    item(row, COL_ANNOTATION)->setText("  " % param->description());
}

void SchemaParamsTable::schemaLoaded(Schema*)
{
    populate();
}

void SchemaParamsTable::customParamCreated(Schema*, Z::Parameter* param)
{
    int row = rowCount();
    setRowCount(row+1);
    createRow(row);
    populateRow(param, row);
    setSelected(param);
    param->addListener(this);
}

void SchemaParamsTable::customParamDeleting(Schema*, Z::Parameter* param)
{
    auto row = findRow(param);
    if (row < 0) return;
    removeRow(row);
    adjustColumns();
}

void SchemaParamsTable::parameterChanged(Z::ParameterBase* param)
{
    Z_PERF_BEGIN("SchemaParamsTable::parameterChanged")

    auto row = findRow(param);
    if (row < 0) return;
    populateRow(reinterpret_cast<Z::Parameter*>(param), row);
    adjustColumns();

    Z_PERF_END
}

void SchemaParamsTable::customParamEdited(Schema*, Z::Parameter* param)
{
    parameterChanged(param);
}

int SchemaParamsTable::findRow(Z::ParameterBase *param)
{
    return schema()->customParams()->indexOf(dynamic_cast<Z::Parameter*>(param));
}

