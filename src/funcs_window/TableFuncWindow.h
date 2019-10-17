#ifndef TABLE_FUNC_WINDOW_H
#define TABLE_FUNC_WINDOW_H

#include "../SchemaWindows.h"

class TableFunction;

class TableFuncWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    explicit TableFuncWindow(TableFunction*);
    ~TableFuncWindow();

    TableFunction* function() const { return _function; }

public slots:
    void update();

private:
    TableFunction* _function;
    int _windowIndex = 0;

    QString displayWindowTitle() const;
};

#endif // TABLE_FUNC_WINDOW_H
