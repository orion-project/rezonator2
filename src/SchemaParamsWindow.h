#ifndef SCHEMA_PARAMS_WINDOW_H
#define SCHEMA_PARAMS_WINDOW_H

#include "SchemaWindows.h"
#include "io/ISchemaWindowStorable.h"

/**
    Implementation of restoreability for @a SchemaParamsWindow.
    Register it in @a ProjectWindow::registerStorableWindows().
*/
namespace SchemaParamsWindowStorable
{
const QString windowType("SchemaParams");
SchemaWindow* createWindow(Schema* schema);
}

class SchemaParamsWindow : public SchemaMdiChild, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static SchemaParamsWindow* create(Schema*);

    ~SchemaParamsWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return SchemaParamsWindowStorable::windowType; }

private slots:
    void createParameter();
    void deleteParameter();
    void setParameterValue();
    void annotateParameter();
    void adjustParameter();

private:
    explicit SchemaParamsWindow(Schema*);

    static SchemaParamsWindow* _instance;

    class SchemaParamsTable* _table;

    QAction *_actnParamAdd, *_actnParamDelete, *_actnParamSet, *_actnParamDescr,
            *_actnParamAdjust;
    QMenu *_windowMenu, *_contextMenu;
    bool _isSettingValueForNewParam = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
};

#endif // SCHEMA_PARAMS_WINDOW_H
