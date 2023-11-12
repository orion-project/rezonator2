#ifndef TABLE_FUNC_WINDOW_H
#define TABLE_FUNC_WINDOW_H

#include "../io/ISchemaWindowStorable.h"
#include "../math/TableFunction.h"
#include "../windows/SchemaWindows.h"

#include <QTableWidget>
#include <QItemDelegate>

QT_BEGIN_NAMESPACE
class QAction;
class QTextBrowser;
QT_END_NAMESPACE

namespace Ori {
namespace Widgets {
class StatusBar;
}}

class FrozenStateButton;
class TableFunction;

//------------------------------------------------------------------------------

class TableFuncPositionColumnItemDelegate : public QItemDelegate
{
public:
    TableFuncPositionColumnItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const override;

private:
    mutable QModelIndex _paintingIndex;
};

//------------------------------------------------------------------------------

class TableFuncResultTable: public QTableWidget
{
    Q_OBJECT

public:
    TableFuncResultTable(const QVector<TableFunction::ColumnDef>& columns);

    bool showT = true;
    bool showS = true;

    void updateColumnTitles();
    void update(const QVector<TableFunction::Result>& results);

    void copy();

private:
    QVector<TableFunction::ColumnDef> _columns;
    QMenu *_contextMenu = nullptr;

    void showContextMenu(const QPoint& pos);
};

//------------------------------------------------------------------------------

class TableFuncWindow : public SchemaMdiChild, public IEditableWindow, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    explicit TableFuncWindow(TableFunction*);
    ~TableFuncWindow() override;

    TableFunction* function() const { return _function; }

    bool configure();

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return QList<QMenu*>() << _menuTable; }

    // Implementation of SchemaListener
    void recalcRequired(Schema*) override { update(); }

    // Implementation of IEditableWindow
    SupportedCommands supportedCommands() override { return EditCmd_Copy | EditCmd_SelectAll; }
    bool canCopy() override { return true; }
    void copy() override { _table->copy(); }
    void selectAll() override { _table->selectAll(); }

    // implementation of ISchemaWindowStorable
    QString storableType() const override { return _function->alias(); }
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;

public slots:
    void update();

private slots:
    void activateModeT();
    void activateModeS();
    void freeze(bool);
    void toggleCalcMediumEnds(bool);
    void toggleCalcEmptySpaces(bool);
    void toggleCalcSpaceMids(bool);

protected:
    TableFunction *_function;
    QMenu *_menuTable;
    QAction *_actnUpdate, *_actnShowT, *_actnShowS, *_actnFreeze, *_actnFrozenInfo,
        *_actnCalcMediumEnds, *_actnCalcEmptySpaces, *_actnCalcSpaceMids;
    FrozenStateButton* _buttonFrozenInfo;
    Ori::Widgets::StatusBar *_statusBar;
    TableFuncResultTable *_table;
    QTextBrowser* _errorView;
    bool _frozen = false;
    bool _needRecalc = false;

    void createActions();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void createContent();

    void showModeTS();
    void updateModeTS();
    void updateTable();
    void updateParamsActions();

    virtual bool configureInternal(const TableFunction::Params&);

    static TableFunction::Params readParams(const QJsonObject& obj);
    static QJsonObject writeParams(const TableFunction::Params& params);
};

#endif // TABLE_FUNC_WINDOW_H
