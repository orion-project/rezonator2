#include "ParamsTreeWidget.h"

#include "Appearance.h"
#include "ElementImagesProvider.h"
#include "../CustomPrefs.h"
#include "../core/Schema.h"
#include "../core/ElementFilter.h"
#include "../core/Utils.h"

#include "helpers/OriDialogs.h"

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>

Z::Parameter* ParamsTreeWidget::selectParamDlg(Schema* schema, const QString& title, const QString& prompt)
{
    Options opts;
    opts.schema = schema;

    ParamsTreeWidget paramsTree(opts);

    if (!prompt.isEmpty())
    {
        auto layout = qobject_cast<QVBoxLayout*>(paramsTree.layout());
        if (layout)
            layout->insertWidget(0, Z::Gui::headerlabel(prompt));
    }

    Ori::Dlg::Dialog dlg(&paramsTree);
    dlg.withTitle(title)
       .withStretchedContent()
       .withInitialSize(CustomPrefs::recentSize("select_param_tree_dlg_size"))
       .withOkSignal(SIGNAL(paramDoubleClicked(Z::Parameter*)));
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

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(_tree);

    populate();

    _tree->expandAll();
    _tree->setColumnCount(2);
    _tree->resizeColumnToContents(0);
    //_tree->resizeColumnToContents(1);
    _tree->headerItem()->setHidden(true);
    connect(_tree, &QTreeWidget::itemDoubleClicked, this, &ParamsTreeWidget::itemDoubleClicked);
}

void ParamsTreeWidget::populate()
{
    for (auto elem : _opts.schema->elements())
        if (!_opts.elemFilter || _opts.elemFilter->check(elem))
        {
            auto iconPath = ElementImagesProvider::instance().iconPath(elem->type());
            addRootItem(elem->displayLabelTitle(), iconPath, elem->params());
        }
    if (_opts.showGlobalParams)
        addRootItem(tr("Globals"), ":/toolbar/parameter", _opts.schema->globalParams());
}

void ParamsTreeWidget::addRootItem(const QString& title, const QString &iconPath, const Z::Parameters &params)
{
    auto f = _tree->font();
    f.setBold(true);

    auto root = new QTreeWidgetItem;
    root->setFont(0, f);
    root->setText(0, title);
    root->setIcon(0, QIcon(iconPath));
    root->setFlags(root->flags() & ~Qt::ItemIsSelectable);

    for (auto param : params)
        if (!_opts.paramFilter || _opts.paramFilter->check(param))
            addParamItem(root, param);

    _tree->addTopLevelItem(root);
}

void ParamsTreeWidget::addParamItem(QTreeWidgetItem* root, Z::Parameter* param)
{
    auto paramLabel = param->displayLabel();
    auto paramName = param->name();

    auto item = new QTreeWidgetItem;
    item->setText(0, QStringLiteral("%1 = %2").arg(paramLabel, param->value().displayStr()));
    item->setText(1, paramName == paramLabel ? param->description() : paramName);
    item->setData(0, Qt::UserRole, ptr2var(param));
    root->addChild(item);
}

Z::Parameter* ParamsTreeWidget::selectedParam() const
{
    auto item = _tree->currentItem();
    if (not item) return nullptr;
    auto varParam = item->data(0, Qt::UserRole);
    if (varParam.isNull()) return nullptr;
    return var2ptr<Z::Parameter*>(varParam);
}

void ParamsTreeWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    auto varParam = item->data(0, Qt::UserRole);
    if (not varParam.isNull())
        emit paramDoubleClicked(var2ptr<Z::Parameter*>(varParam));
}
