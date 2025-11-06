#ifndef CUSTOM_CODE_WINDOW_H
#define CUSTOM_CODE_WINDOW_H

#include "../windows/CodeEditorWindow.h"
#include "../io/ISchemaWindowStorable.h"

/**
    Implementation of restoreability for CustomCodeWindow.
    Register it in ProjectWindow::registerStorableWindows().
*/
namespace CustomCodeWindowStorable
{
inline QString windowType() { return "CustomCode"; }
SchemaWindow* createWindow(Schema* schema);
}

/**
    Simple function window that can run custom python code
    without any visual representation of results.
*/
class CustomCodeWindow : public CodeEditorWindow, public ISchemaWindowStorable
{
    Q_OBJECT

public:
    static CustomCodeWindow* create(Schema*, const QString &codeTemplate = {});

    // inherits from BasicMdiChild
    QString helpTopic() const override { return "func_custom_code.html"; }

    // inherits from ISchemaWindowStorable
    QString storableType() const override { return CustomCodeWindowStorable::windowType(); }
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;

protected:
    void closeEvent(QCloseEvent* ce) override;
    
    void runCode() override;

private:
    QString _moduleName;

    explicit CustomCodeWindow(Schema*);
};

#endif // CUSTOM_CODE_WINDOW_H
