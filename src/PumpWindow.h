#ifndef PUMP_WINDOW_H
#define PUMP_WINDOW_H

#include "SchemaWindows.h"
#include "io/ISchemaWindowStorable.h"

namespace PumpWindowStorable
{
const QString windowType("PumpsWindow");
SchemaWindow* createWindow(Schema* schema);
}

class PumpWindow : public SchemaMdiChild, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static PumpWindow* create(Schema*);

    ~PumpWindow();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return PumpWindowStorable::windowType; }

private:
    explicit PumpWindow(Schema*owner);

    static PumpWindow* _instance;

    class SchemaPumpsTable* _table;

    QAction *_actnPumpAdd, *_actnPumpDelete, *_actnPumpEdit, *_actnPumpActivate;
    QMenu *_windowMenu, *_contextMenu;
    bool _isEditingNewPump = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
};

#endif // PUMP_WINDOW_H
