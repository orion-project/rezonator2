#ifndef CUSTOM_TABLE_FUNC_WINDOW_H
#define CUSTOM_TABLE_FUNC_WINDOW_H

#include "../funcs/TableFuncWindow.h"
#include "../math/CustomTableFunction.h"

class CustomTableCodeWindow;

class CustomTableFuncWindow final : public TableFuncWindow
{
    Q_OBJECT

public:
    explicit CustomTableFuncWindow(Schema*);
    
    CustomTableFunction* function() const { return dynamic_cast<CustomTableFunction*>(_function); }

    // implementation of ISchemaWindowStorable
    bool storableRead(const QJsonObject& root, Z::Report*) override;
    bool storableWrite(QJsonObject& root, Z::Report*) override;
    
protected:
    void closeEvent(QCloseEvent* ce) override;

    void beforeUpdate() override;
    void afterUpdate() override;
    
private:
    QAction *_actnShowCode;
    QPointer<CustomTableCodeWindow> _codeWindow;

    void showCode();
};

#endif // CUSTOM_TABLE_FUNC_WINDOW_H
