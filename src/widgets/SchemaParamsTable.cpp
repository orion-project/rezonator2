#include "SchemaParamsTable.h"

#include "Appearance.h"
#include "RichTextItemDelegate.h"

#include <QHeaderView>
#include <QMenu>

SchemaParamsTable::SchemaParamsTable(Schema *schema, QWidget *parent) : QTableWidget(0, COL_COUNT, parent), _schema(schema)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemDelegateForColumn(COL_ALIAS, new RichTextItemDelegate(this));
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(COL_IMAGE, QHeaderView::Fixed);
    horizontalHeader()->setMinimumSectionSize(_iconSize+4);
    horizontalHeader()->resizeSection(COL_IMAGE, _iconSize+4);
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
    Z::Gui::setSymbolFont(it);
    Z::Gui::setFontStyle(it, false); // make it bold in html content
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_ALIAS, it);

    it = new QTableWidgetItem();
    Z::Gui::setValueFont(it);
    it->setTextAlignment(Qt::AlignCenter);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_VALUE, it);

    it = new QTableWidgetItem();
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setItem(row, COL_ANNOTATION, it);
}

void SchemaParamsTable::populateRow(Z::Parameter *param, int row)
{
    // Parameter alias and formula
    QString formulaDescr;
    auto formula = schema()->formulas()->get(param);
    if (formula && !formula->deps().isEmpty())
    {
        QStringList params;
        for (auto dep : formula->deps())
            params << dep->alias();
        formulaDescr = QString(" <i>= f(%1)</i>").arg(params.join(", "));
    }
    item(row, COL_ALIAS)->setText(QString("<center><b><span style='color:%1'>%2</span></b>%3</center>")
                                  .arg(Z::Gui::globalParamColorHtml(), param->alias(), formulaDescr));

    // Parameter icon
    auto iconPath = (param->valueDriver() == Z::ParamValueDriver::Formula)
        ? ":/toolbar/param_formula" : ":/toolbar/parameter";
    item(row, COL_IMAGE)->setData(Qt::DecorationRole, QIcon(iconPath).pixmap(_iconSize, _iconSize));

    // Parameter value
    auto it = item(row, COL_VALUE);
    auto f = it->font();
    f.setItalic(param->valueDriver() == Z::ParamValueDriver::Formula);
    it->setFont(f);
    it->setText(param->value().displayStr());

    // Parameter annotation
    item(row, COL_ANNOTATION)->setText(param->description());
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
    auto row = findRow(param);
    if (row < 0) return;
    populateRow(reinterpret_cast<Z::Parameter*>(param), row);
    adjustColumns();
}

void SchemaParamsTable::customParamEdited(Schema*, Z::Parameter* param)
{
    parameterChanged(param);
}

int SchemaParamsTable::findRow(Z::ParameterBase *param)
{
    return schema()->customParams()->indexOf(dynamic_cast<Z::Parameter*>(param));
}

