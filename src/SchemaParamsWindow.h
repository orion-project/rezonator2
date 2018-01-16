#ifndef SCHEMA_PARAMS_WINDOW_H
#define SCHEMA_PARAMS_WINDOW_H

#include "SchemaWindows.h"
#include "core/Schema.h"

class SchemaParamsWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    static SchemaParamsWindow* create(Schema*);

private:
    explicit SchemaParamsWindow(Schema*);
    ~SchemaParamsWindow();

    static SchemaParamsWindow* _instance;
};

#endif // SCHEMA_PARAMS_WINDOW_H
