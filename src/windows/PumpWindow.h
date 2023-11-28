#ifndef PUMP_WINDOW_H
#define PUMP_WINDOW_H

#include "../io/ISchemaWindowStorable.h"
#include "../windows/SchemaWindows.h"

#include <QTableWidget>

namespace Ori {
namespace Widgets {
class StatusBar;
}}

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
    enum { COL_COLOR, COL_IMAGE, COL_ACTIVE, COL_LABEL, COL_PARAMS, COL_TITLE, COL_COUNT };

    PumpsTable(Schema *schema, QWidget *parent = nullptr);

    void setContextMenu(QMenu *menu) { _contextMenu = menu; }

    Schema* schema() const { return _schema; }

    PumpParams* selected() const;
    QList<PumpParams*> selection() const;
    QList<int> selectedRows() const;
    void setSelected(PumpParams*);

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override;
    void pumpCreated(Schema*, PumpParams*) override;
    void pumpChanged(Schema*, PumpParams*) override;
    void pumpCustomized(Schema*, PumpParams*) override;
    void pumpDeleting(Schema*, PumpParams*) override;

signals:
    void pumpDoubleClicked(PumpParams*, int);

private slots:
    void doubleClicked(class QTableWidgetItem*);
    void showContextMenu(const QPoint&);

private:
    Schema *_schema;
    QMenu *_contextMenu = nullptr;

    void adjustColumns();
    void populate();
    void createRow(int row);
    void populateRow(PumpParams *pump, int row);
    int findRow(PumpParams *pump);

    const int _iconSize = 24;
};

//------------------------------------------------------------------------------

class PumpWindow : public SchemaMdiChild,
                   public ISchemaWindowStorable,
                   public IEditableWindow,
                   public IShortcutListener
{
    Q_OBJECT

public:
    static PumpWindow* create(Schema*);

    ~PumpWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }
    QString helpTopic() const override { return "pumps_window.html"; }

    // inherits from SchemaListener
    void schemaLoaded(Schema*) override { showStatusInfo(); }

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return PumpWindowStorable::windowType; }

    // inherits from EditableWindow
    SupportedCommands supportedCommands() override {
        return EditCmd_Copy | EditCmd_Paste | EditCmd_SelectAll; }
    bool canCopy() override;
    bool canPaste() override { return true; }
    void selectAll() override;

    // inherits from IShortcutListener
    void shortcutEnterPressed() override;

public slots:
    void createPump();
    void deletePump();
    void editPump();
    void activatePump();
    void clonePump();
    void copy() override;
    void paste() override;
    void setPumpColor();
    void pumpDoubleClicked(PumpParams*, int);

protected:
    explicit PumpWindow(Schema*owner);

    virtual PumpParams* makeNewPumpDlg();
    virtual bool editPumpDlg(PumpParams*);
    virtual PumpParams* selectedPump() const;

private:
    static PumpWindow* _instance;

    PumpsTable* _table;
    QAction *_actnPumpAdd, *_actnPumpDelete, *_actnPumpEdit, *_actnPumpActivate, *_actnPumpClone,
        *_actnPumpCopy, *_actnPumpPaste, *_actnPumpColor;
    QMenu *_windowMenu, *_contextMenu;
    Ori::Widgets::StatusBar* _statusBar;
    bool _isEditingNewPump = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void showStatusInfo();
    void addNewPump(PumpParams* pump);
};

#endif // PUMP_WINDOW_H
