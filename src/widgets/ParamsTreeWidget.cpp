#include "ParamsTreeWidget.h"

#include "Appearance.h"
#include "ElementImagesProvider.h"
#include "RichTextItemDelegate.h"
#include "../CustomPrefs.h"
#include "../core/Schema.h"
#include "../core/ElementFilter.h"
#include "../core/Utils.h"
#include "../funcs/FormatInfo.h"

#include "helpers/OriDialogs.h"

#include <QAbstractButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace  {
enum {
    COL_TITLE,
    COL_DESCR,

    COL_COUNT
};

Z::Parameter* paramOfItem(QTreeWidgetItem *item)
{
    return item ? var2ptr<Z::Parameter*>(item->data(COL_TITLE, Qt::UserRole)) : nullptr;
}

}

Z::Parameter* ParamsTreeWidget::selectParamDlg(Options opts)
{
    ParamsTreeWidget paramsTree(opts);

    if (!opts.dialogPrompt.isEmpty())
    {
        auto layout = qobject_cast<QVBoxLayout*>(paramsTree.layout());
        if (layout)
            layout->insertWidget(0, Z::Gui::headerlabel(opts.dialogPrompt));
    }

    Ori::Dlg::Dialog dlg(&paramsTree);
    dlg.withTitle(opts.dialogTitle)
       .withStretchedContent()
       .withInitialSize(CustomPrefs::recentSize("select_param_tree_dlg_size"))
       .withOkSignal(SIGNAL(paramDoubleClicked(Z::Parameter*)))
       .withOnDlgReady([&](){
            if (!dlg.okButton()) return;
            connect(&paramsTree, &ParamsTreeWidget::paramSelected, [&](Z::Parameter* param){
                dlg.okButton()->setEnabled(param);
            });
        });
    if (dlg.exec())
    {
        CustomPrefs::setRecentSize("select_param_tree_dlg_size", dlg.size());
        return paramsTree.selectedParam();
    }
    return nullptr;
}


ParamsTreeWidget::ParamsTreeWidget(Options opts, QWidget *parent) : QWidget(parent), _opts(opts)
{
    _tree = new QTreeWidget;
    _tree->setColumnCount(COL_COUNT);
    _tree->setHeaderHidden(true);
    _tree->setUniformRowHeights(true);
    _tree->setItemDelegate(new RichTextItemDelegate());
    connect(_tree, &QTreeWidget::itemDoubleClicked, this, &ParamsTreeWidget::itemDoubleClicked);
    connect(_tree, &QTreeWidget::currentItemChanged, this, &ParamsTreeWidget::currentItemChanged);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_tree);

    populate();
    _tree->expandAll();
    _tree->resizeColumnToContents(COL_TITLE);
}

void ParamsTreeWidget::populate()
{
    for (auto elem : _opts.schema->elements())
        if (!_opts.elemFilter || _opts.elemFilter->check(elem))
        {
            auto iconPath = ElementImagesProvider::instance().iconPath(elem->type());
            addRootItem(elem->displayLabelTitle(), iconPath, elem->params(), true);
        }
    if (_opts.showGlobalParams)
        addRootItem(tr("Globals"), ":/toolbar/parameter", _opts.schema->globalParams(), false);
}

void ParamsTreeWidget::addRootItem(const QString& title, const QString &iconPath, const Z::Parameters &params, bool isElement)
{
    QVector<QTreeWidgetItem*> items;
    for (auto param : params)
        if (!_opts.ignoreList.contains(param))
            if (!_opts.paramFilter || _opts.paramFilter->check(param))
                items << addParamItem(param, isElement);

    if (items.isEmpty()) return;

    auto f = _tree->font();
    f.setBold(true);

    auto root = new QTreeWidgetItem;
    root->setFont(COL_TITLE, f);
    root->setText(COL_TITLE, title);
    root->setIcon(COL_TITLE, QIcon(iconPath));
    root->setFirstColumnSpanned(true);
    for (auto item : items)
        root->addChild(item);
    _tree->addTopLevelItem(root);
}

struct ParamInfo
{
    Schema* schema;
    Z::Parameter* param;
    bool isElement;
};

QTreeWidgetItem* ParamsTreeWidget::addParamItem(Z::Parameter* param, bool isElement)
{
    Z::Format::FormatParam f;
    f.includeValue = true;
    f.smallName = true;
    f.isElement = isElement;
    f.schema = _opts.schema;

    auto item = new QTreeWidgetItem;
    item->setText(COL_TITLE, f.format(param));
    auto descr = isElement ? param->name() : param->description();
    item->setText(COL_DESCR, descr);
    item->setToolTip(COL_DESCR, descr);
    item->setData(COL_TITLE, Qt::UserRole, ptr2var(param));
    return item;
}

Z::Parameter* ParamsTreeWidget::selectedParam() const
{
    return paramOfItem(_tree->currentItem());
}

void ParamsTreeWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    auto param = paramOfItem(item);
    if (param) emit paramDoubleClicked(param);
}

void ParamsTreeWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    emit paramSelected(paramOfItem(current));
}

