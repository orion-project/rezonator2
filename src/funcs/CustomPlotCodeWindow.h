#ifndef CUSTOM_PLOT_CODE_WINDOW_H
#define CUSTOM_PLOT_CODE_WINDOW_H

#include "../math/CustomPlotFunction.h"
#include "../windows/CodeEditorWindow.h"

class CustomPlotCodeWindow : public CodeEditorWindow
{
    Q_OBJECT

public:
    CustomPlotCodeWindow(CustomPlotFunction *func, std::function<void()> updatePlotWindow);
    ~CustomPlotCodeWindow();
    
    void showResult();
    
    // inherits from BasicMdiChild
    QString helpTopic() const override { return _function->helpTopic(); }
    
protected:
    void runCode() override;
    
private:
    CustomPlotFunction *_function;
    std::function<void()> _updatePlotWindow;
};

#endif // CUSTOM_PLOT_CODE_WINDOW_H
