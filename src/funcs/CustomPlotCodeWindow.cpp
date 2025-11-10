#include "CustomPlotCodeWindow.h"

#include <QTimer>

CustomPlotCodeWindow::CustomPlotCodeWindow(CustomPlotFunction *func, std::function<void ()> updatePlotWindow)
    : CodeEditorWindow(func->schema()), _function(func), _updatePlotWindow(updatePlotWindow)
{
    setCode(_function->code());
    _function->setPrintFunc([this](const QString &s){ logInfo(s); });
    
    QTimer::singleShot(0, this, [this]{ runCode(); });
}

CustomPlotCodeWindow::~CustomPlotCodeWindow()
{
    _function->setPrintFunc(nullptr);
}

void CustomPlotCodeWindow::runCode()
{
    _updatePlotWindow();
}

void CustomPlotCodeWindow::showResult()
{
    if (!_function->ok())
        logError(_function->errorLog(), _function->errorLine());
}
