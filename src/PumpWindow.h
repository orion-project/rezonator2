#ifndef PUMP_WINDOW_H
#define PUMP_WINDOW_H

#include "SchemaWindows.h"
#include "io/ISchemaWindowStorable.h"

#include <QTableWidget>

/**
    Implementation of restoreability for @a PumpWindow.
    Register it in @a ProjectWindow::registerStorableWindows().
*/
namespace PumpWindowStorable
{
const QString windowType("PumpsWindow");
SchemaWindow* createWindow(Schema* schema);
}

//------------------------------------------------------------------------------

class PumpsTable : public QTableWidget, public SchemaListener
{
    Q_OBJECT

public:
    PumpsTable(Schema *schema, QWidget *parent = 0);

    void setContextMenu(QMenu *menu) { _contextMenu = menu; }

    Schema* schema() const { return _schema; }

    Z::PumpParams* selected() const;
    void setSelected(Z::PumpParams*);

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override;
    void pumpCreated(Schema*, Z::PumpParams*) override;
    void pumpChanged(Schema*, Z::PumpParams*) override;
    void pumpDeleting(Schema*, Z::PumpParams*) override;

signals:
    void doubleClicked(Z::PumpParams*);

private slots:
    void doubleClicked(class QTableWidgetItem*);
    void showContextMenu(const QPoint&);

private:
    enum { COL_IMAGE, COL_LABEL, COL_PARAMS, COL_TITLE, COL_COUNT };

    Schema *_schema;
    QMenu *_contextMenu = nullptr;

    void adjustColumns();
    void populate();
    void createRow(int row);
    void populateRow(Z::PumpParams *pump, int row);
    int findRow(Z::PumpParams *pump);

    const int _iconSize = 24;
};

//------------------------------------------------------------------------------

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

private slots:
    void createPump();
    void deletePump();
    void editPump();
    void activatePump();

private:
    explicit PumpWindow(Schema*owner);
    static PumpWindow* _instance;

    PumpsTable* _table;
    QAction *_actnPumpAdd, *_actnPumpDelete, *_actnPumpEdit, *_actnPumpActivate;
    QMenu *_windowMenu, *_contextMenu;
    bool _isEditingNewPump = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
};

#endif // PUMP_WINDOW_H
