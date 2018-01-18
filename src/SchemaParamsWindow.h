#ifndef SCHEMA_PARAMS_WINDOW_H
#define SCHEMA_PARAMS_WINDOW_H

#include "SchemaWindows.h"
#include "core/Schema.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

class SchemaParamsWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    static SchemaParamsWindow* create(Schema*);

    ~SchemaParamsWindow();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }

private slots:
    void createParameter();
    void deleteParameter();
    void setParameterValue();

private:
    explicit SchemaParamsWindow(Schema*);

    static SchemaParamsWindow* _instance;

    class SchemaParamsTable* _table;

    QAction *_actnParamAdd, *_actnParamDelete, *_actnParamSet;
    QMenu *_windowMenu, *_contextMenu;

    void createActions();
    void createMenuBar();
    void createToolBar();
};

#endif // SCHEMA_PARAMS_WINDOW_H
