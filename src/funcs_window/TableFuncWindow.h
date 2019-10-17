#ifndef TABLE_FUNC_WINDOW_H
#define TABLE_FUNC_WINDOW_H

#include "../SchemaWindows.h"
#include "../funcs/TableFunction.h"

#include <QTableWidget>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
class StatusBar;
}}

class FrozenStateButton;
class TableFunction;


class TableFuncResultTable: public QTableWidget
{
    Q_OBJECT

public:
    TableFuncResultTable(const QVector<TableFunction::ColumnDef>& columns);

    void adjustColumns();
    void updateColumnTitles(bool t, bool s);

private:
    QVector<TableFunction::ColumnDef> _columns;
};


class TableFuncWindow : public SchemaMdiChild
{
    Q_OBJECT

public:
    explicit TableFuncWindow(TableFunction*);
    ~TableFuncWindow() override;

    TableFunction* function() const { return _function; }

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return QList<QMenu*>() << _menuTable; }

public slots:
    void update();

private slots:
    void activateModeT();
    void activateModeS();
    void freeze(bool);

private:
    TableFunction *_function;
    QMenu *_menuTable;
    QAction *_actnUpdate, *_actnShowT, *_actnShowS, *_actnFreeze, *_actnFrozenInfo;
    FrozenStateButton* _buttonFrozenInfo;
    Ori::Widgets::StatusBar *_statusBar;
    TableFuncResultTable *_table;
    bool _frozen = false;
    bool _needRecalc = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void showStatusError(const QString &message);
    void clearStatusInfo();

    void showModeTS();
    void updateModeTS();
    void updateTable();
};

#endif // TABLE_FUNC_WINDOW_H
