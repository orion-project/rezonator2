#ifndef FUNC_EDITOR_WINDOW_H
#define FUNC_EDITOR_WINDOW_H

#include "../io/ISchemaWindowStorable.h"
#include "../windows/SchemaWindows.h"

#include <QtCore/QProcess>

/**
    Implementation of restoreability for CustomFunctionWindow.
    Register it in ProjectWindow::registerStorableWindows().
*/
namespace FuncEditorWindowStorable
{
inline QString windowType() { return "FuncEditor"; }
SchemaWindow* createWindow(Schema* schema);
}

class QPlainTextEdit;

class FuncEditorWindow : public SchemaMdiChild, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static FuncEditorWindow* create(Schema*);

    ~FuncEditorWindow() override;

    // inherits from BasicMdiChild
    QList<QMenu*> menus() override { return { _windowMenu }; }
    QString helpTopic() const override { return ""; } // TODO: Add help topic

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return FuncEditorWindowStorable::windowType(); }
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;

private slots:
    void checkFunction();
    void handlePythonOutput();
    void handlePythonError();
    void handlePythonFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    explicit FuncEditorWindow(Schema*);

    QPlainTextEdit* _editor;
    QPlainTextEdit* _log;
    QAction* _actnCheck;
    QMenu* _windowMenu;
    QProcess* _pythonProcess;
    QString _tempScriptPath;
    
    QString getPythonInterpreterPath() const;
    void cleanupTempFiles();
    
    void createActions();
    void createMenuBar();
    void createToolBar();
    void createContent();
};

#endif // FUNC_EDITOR_WINDOW_H
