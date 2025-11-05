#ifndef FUNC_EDITOR_WINDOW_H
#define FUNC_EDITOR_WINDOW_H

#include "CodeEditorWindow.h"
#include "../io/ISchemaWindowStorable.h"

/**
    Implementation of restoreability for FuncEditorWindow.
    Register it in ProjectWindow::registerStorableWindows().
*/
namespace FuncEditorWindowStorable
{
inline QString windowType() { return "FuncEditor"; }
SchemaWindow* createWindow(Schema* schema);
}

class FuncEditorWindow : public CodeEditorWindow, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static FuncEditorWindow* create(Schema*, const QString &codeTemplate = {});

    // inherits from BasicMdiChild
    QString helpTopic() const override { return ""; } // TODO: Add help topic

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return FuncEditorWindowStorable::windowType(); }
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;

protected:
    void closeEvent(QCloseEvent* ce) override;
    
    void runCode() override;

private:
    QString _moduleName;

    explicit FuncEditorWindow(Schema*);
};

#endif // FUNC_EDITOR_WINDOW_H
