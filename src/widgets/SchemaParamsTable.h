#ifndef SCHEMA_PARAMS_TABLE_H
#define SCHEMA_PARAMS_TABLE_H

#include <QTableWidget>

#include "../core/Schema.h"

class SchemaParamsTable : public QTableWidget, public SchemaListener
{
public:
    SchemaParamsTable(Schema *schema, QWidget *parent = 0);

private:
    enum { COL_IMAGE, COL_NAME, COL_VALUE, COL_TITLE, COL_COUNT };

    Schema *_schema;
    QMenu *_contextMenu = nullptr;

    void adjustColumns();
};

#endif // SCHEMA_PARAMS_TABLE_H
