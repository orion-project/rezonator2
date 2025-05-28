#ifndef SCHEMA_PARAMS_TABLE_H
#define SCHEMA_PARAMS_TABLE_H

#include <QTableView>

#include "../core/Schema.h"

class SchemaParamsTableModel;

class SchemaParamsTable : public QTableView
{
    Q_OBJECT

public:
    SchemaParamsTable(Schema *schema, QWidget *parent = nullptr);

    void setContextMenu(QMenu *menu) { _contextMenu = menu; }

    Schema* schema() const { return _schema; }

    Z::Parameter* selected() const;
    void setSelected(Z::Parameter*);

    int currentRow() const;

signals:
    void paramDoubleClicked(Z::Parameter*);

private:
    Schema *_schema;
    QMenu *_contextMenu = nullptr;
    SchemaParamsTableModel *_model;

    void indexDoubleClicked(const QModelIndex &index);
    void showContextMenu(const QPoint&);
};

#endif // SCHEMA_PARAMS_TABLE_H
