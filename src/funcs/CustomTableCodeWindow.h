#ifndef CUSTOM_TABLE_CODE_WINDOW_H
#define CUSTOM_TABLE_CODE_WINDOW_H

#include "../math/CustomTableFunction.h"
#include "../windows/CodeEditorWindow.h"

class CustomTableCodeWindow : public CodeEditorWindow
{
    Q_OBJECT

public:
    CustomTableCodeWindow(CustomTableFunction *func, std::function<void()> updateTableWindow);
    ~CustomTableCodeWindow();
    
    void showResult();
    
protected:
    void runCode();
    
private:
    CustomTableFunction *_function;
    std::function<void()> _updateTableWindow;
};

#endif // CUSTOM_TABLE_CODE_WINDOW_H
