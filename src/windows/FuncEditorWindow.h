#ifndef FUNC_EDITOR_WINDOW_H
#define FUNC_EDITOR_WINDOW_H

#include "../io/ISchemaWindowStorable.h"
#include "../windows/SchemaWindows.h"

class QPlainTextEdit;

/**
    Implementation of restoreability for FuncEditorWindow.
    Register it in ProjectWindow::registerStorableWindows().
*/
namespace FuncEditorWindowStorable
{
inline QString windowType() { return "FuncEditor"; }
SchemaWindow* createWindow(Schema* schema);
}

class FuncEditorWindow : public SchemaMdiChild, public ISchemaWindowStorable, public IEditableWindow
{
    Q_OBJECT

public:
    static FuncEditorWindow* create(Schema*);

    ~FuncEditorWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }
    QString helpTopic() const override { return ""; } // TODO: Add help topic

    // inherits from IEditableWindow
    SupportedCommands supportedCommands() override {
        return EditCmd_Undo | EditCmd_Redo | EditCmd_Cut | EditCmd_Copy | EditCmd_Paste | EditCmd_SelectAll; }
    bool canUndo() override;
    bool canRedo() override;
    bool canCut() override;
    bool canCopy() override;
    bool canPaste() override;
    void undo() override;
    void redo() override;
    void cut() override;
    void copy() override;
    void paste() override;
    void selectAll() override;

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return FuncEditorWindowStorable::windowType(); }
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;

private:
    explicit FuncEditorWindow(Schema*);

    QPlainTextEdit* _editor;
    QPlainTextEdit* _log;
    QAction *_actionUndo, *_actionRedo, *_actionCut, *_actionCopy, *_actionPaste, *_actnRun;
    QMenu* _windowMenu;
    
    void createActions();
    void createMenuBar();
    void createToolBar();

    void run();

    void markModified(bool m);

    void logInfo(const QString &msg);
    void logError(const QString &msg);
};

#endif // FUNC_EDITOR_WINDOW_H
