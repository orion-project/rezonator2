#ifndef SCHEMA_PARAMS_TABLE_H
#define SCHEMA_PARAMS_TABLE_H

#include <QTableWidget>

#include "../core/Schema.h"

class SchemaParamsTable : public QTableWidget, public SchemaListener
{
    Q_OBJECT

public:
    SchemaParamsTable(Schema *schema, QWidget *parent = 0);

    void setContextMenu(QMenu *menu) { _contextMenu = menu; }

    Schema* schema() const { return _schema; }

    Z::Parameter* selected() const;
    void setSelected(Z::Parameter*);

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override;

    void parameterCreated(Z::Parameter*);

signals:
    void doubleClicked(Z::Parameter*);

private slots:
    void doubleClicked(class QTableWidgetItem*);
    void showContextMenu(const QPoint&);

private:
    enum { COL_IMAGE, COL_ALIAS, COL_VALUE, COL_ANNOTATION, COL_COUNT };

    Schema *_schema;
    QMenu *_contextMenu = nullptr;

    void adjustColumns();
    void populate();
    void createRow(Z::Parameter *param, int row);
    void populateRow(Z::Parameter *param, int row);
};

#endif // SCHEMA_PARAMS_TABLE_H
