#include "ParamsTreeWidget.h"

#include "RichTextItemDelegate.h"
#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../app/HelpSystem.h"
#include "../core/Schema.h"
#include "../core/Utils.h"
#include "../math/FormatInfo.h"

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
            layout->insertWidget(0, Z::Gui::makeHeaderLabel(opts.dialogPrompt));
    }

    Ori::Dlg::Dialog dlg(&paramsTree, false);
    dlg.withTitle(opts.dialogTitle)
       .withStretchedContent()
       .withInitialSize(RecentData::getSize("select_param_tree_dlg_size"))
       .withOkSignal(SIGNAL(paramDoubleClicked(Z::Parameter*)))
       .withOnDlgReady([&](){
            if (!dlg.okButton()) return;
            connect(&paramsTree, &ParamsTreeWidget::paramSelected, [&](Z::Parameter* param){
                dlg.okButton()->setEnabled(param);
            });
        });
    if (!opts.helpTopic.isEmpty())
        dlg.withOnHelp([&opts]{
            Z::HelpSystem::instance()->showTopic(opts.helpTopic);
        });
    bool ok = dlg.exec();
    RecentData::setSize("select_param_tree_dlg_size", dlg.size());
    return ok ? paramsTree.selectedParam() : nullptr;
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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_tree);

    populate();
    _tree->expandAll();
    _tree->resizeColumnToContents(COL_TITLE);
}

void ParamsTreeWidget::populate()
{
    for (auto elem : _opts.schema->elements())
        if (!_opts.elemFilter || _opts.elemFilter->check(elem))
            addRootItem(elem);
    if (_opts.showGlobalParams)
        addRootItem(nullptr);
}

void ParamsTreeWidget::addRootItem(Element* elem)
{
    QVector<QTreeWidgetItem*> items;
    auto params = elem ? elem->params() : _opts.schema->globalParams();
    for (auto param : params)
        if (!_opts.ignoreList.contains(param))
            if (!_opts.paramFilter || _opts.paramFilter->check(param))
                items << addParamItem(param, elem);

    if (items.isEmpty()) return;

    auto makeRootItemText = [](const QString& text){
        // RichTextItemDelegate (QTextDocument inside it) applyes default system text color for plain text
        // and ignores QSS overrides. The text gets black on the light system theme, which is ok,
        // but on the dark system theme it gets white, which contradicts with custom light QSS styles.
        // Sa have to make dummy HTML, because a manually generated stylesheet is used for HTML.
        return QStringLiteral("<span>%1</span>").arg(text);
    };

    auto root = new QTreeWidgetItem;
    root->setBackground(COL_TITLE, Z::Gui::darkPaperColor());
    root->setBackground(COL_DESCR, Z::Gui::darkPaperColor());
    if (elem)
    {
        root->setText(COL_TITLE, makeRootItemText(elem->displayLabel()));
        root->setFont(COL_TITLE, Z::Gui::ElemLabelFont().get());
        root->setText(COL_DESCR, makeRootItemText(elem->title()));
    }
    else
    {
        root->setText(COL_TITLE, makeRootItemText(tr("Globals")));
        root->setFont(COL_TITLE, Z::Gui::ValueFont().get());
    }
    Q_FOREACH(auto item, items)
        root->addChild(item);
    _tree->addTopLevelItem(root);
}

QTreeWidgetItem* ParamsTreeWidget::addParamItem(Z::Parameter* param, bool isElement)
{
    Z::Format::FormatParam f;
    f.includeValue = true;
    f.isElement = isElement;
    f.schema = _opts.schema;

    auto item = new QTreeWidgetItem;
    item->setText(COL_TITLE, f.format(param));
    auto descr = isElement ? param->name() : param->description();
    item->setText(COL_DESCR, QStringLiteral("<span style='color:%1'>%2</span>")
        .arg(Z::Gui::mutedTextColor().name(), descr));
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
    Q_UNUSED(column)
    auto param = paramOfItem(item);
    if (param) emit paramDoubleClicked(param);
}

void ParamsTreeWidget::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    emit paramSelected(paramOfItem(current));
}

