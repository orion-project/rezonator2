#ifndef PARAMS_TREE_WIDGET_H
#define PARAMS_TREE_WIDGET_H

#include <QWidget>

#include "../core/Parameters.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class Schema;
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
    };

    explicit ParamsTreeWidget(Options opts, QWidget *parent = nullptr);

    static Z::Parameter* selectParamDlg(Schema* schema, const QString& title, const QString& prompt = QString());

    Z::Parameter* selectedParam() const;

signals:
    void paramDoubleClicked(Z::Parameter* param);

private:
    Options _opts;
    QTreeWidget* _tree;

    void populate();
    void addRootItem(const QString& title, const QString& iconPath, const Z::Parameters& params);
    void addParamItem(QTreeWidgetItem* root, Z::Parameter* param);
    void itemDoubleClicked(QTreeWidgetItem *item, int column);

};

#endif // PARAMS_TREE_WIDGET_H
