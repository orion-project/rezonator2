#ifndef CUSTOM_TABLE_FUNC_WINDOW_H
#define CUSTOM_TABLE_FUNC_WINDOW_H

#include "../funcs/TableFuncWindow.h"
#include "../math/CustomTableFunction.h"

class CodeEditorWindow;

class CustomTableFuncWindow final : public TableFuncWindow
{
    Q_OBJECT

public:
    explicit CustomTableFuncWindow(Schema*);
    
    CustomTableFunction* function() const { return dynamic_cast<CustomTableFunction*>(_function); }

    // implementation of ISchemaWindowStorable
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;
    
private:
    QAction *_actnShowCode;
    QPointer<CodeEditorWindow> _codeWindow;

    void showCode();
};

#endif // CUSTOM_TABLE_FUNC_WINDOW_H
