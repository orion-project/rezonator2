#ifndef PARAMS_TREE_WIDGET_H
#define PARAMS_TREE_WIDGET_H

#include <QWidget>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class Schema;
class Element;
class ElementFilter;

class ParamsTreeWidget : public QWidget
{
    Q_OBJECT

public:
    struct Options
    {
        Schema* schema;
        ElementFilter *elemFilter = nullptr;
        Z::ParameterFilter* paramFilter = nullptr;
        bool showGlobalParams = true;
        QString dialogTitle;
        QString dialogPrompt;
        Z::Parameters ignoreList;
    };

    explicit ParamsTreeWidget(Options opts, QWidget *parent = nullptr);

    static Z::Parameter* selectParamDlg(Options opts);

    Z::Parameter* selectedParam() const;

signals:
    void paramDoubleClicked(Z::Parameter* param);
    void paramSelected(Z::Parameter* param);

private:
    Options _opts;
    QTreeWidget* _tree;

    void populate();
    void addRootItem(Element* elem);
    QTreeWidgetItem *addParamItem(Z::Parameter* param, bool isElement);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif // PARAMS_TREE_WIDGET_H
