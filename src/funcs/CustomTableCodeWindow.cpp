#include "CustomTableCodeWindow.h"

#include <QTimer>

CustomTableCodeWindow::CustomTableCodeWindow(CustomTableFunction *func, std::function<void ()> updateTableWindow)
    : CodeEditorWindow(func->schema()), _function(func), _updateTableWindow(updateTableWindow)
{
    setCode(_function->code());
    _function->setPrintFunc([this](const QString &s){ logInfo(s); });
    
    QTimer::singleShot(0, this, [this]{ runCode(); });
}

CustomTableCodeWindow::~CustomTableCodeWindow()
{
    _function->setPrintFunc(nullptr);
}

void CustomTableCodeWindow::runCode()
{
    _updateTableWindow();
}

void CustomTableCodeWindow::showResult()
{
    if (!_function->ok())
        logError(_function->errorLog(), _function->errorLine());
}
