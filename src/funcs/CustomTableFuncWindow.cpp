#include "CustomTableFuncWindow.h"

#include "../app/MessageBus.h"
#include "../windows/CodeEditorWindow.h"

#include "helpers/OriWidgets.h"

#include <QJsonObject>
#include <QMenu>
#include <QToolBar>

#define A_ Ori::Gui::action

CustomTableFuncWindow::CustomTableFuncWindow(Schema* schema): TableFuncWindow(new CustomTableFunction(schema))
{
    _actnShowCode = A_(tr("Show Code"), this, &CustomTableFuncWindow::showCode, ":/toolbar/python_framed");
    
    _menuTable->addSeparator();
    _menuTable->addAction(_actnShowCode);
    
    toolbar()->addSeparator();
    toolbar()->addAction(_actnShowCode);
}

bool CustomTableFuncWindow::storableRead(const QJsonObject &root, Z::Report *report)
{
    function()->setCode(root["code"].toString());

    return TableFuncWindow::storableRead(root, report);
}

bool CustomTableFuncWindow::storableWrite(QJsonObject &root, Z::Report *report)
{
    if (_codeWindow)
        function()->setCode(_codeWindow->code());

    root["code"] = function()->code();
    
    return TableFuncWindow::storableWrite(root, report);
}

void CustomTableFuncWindow::showCode()
{
    if (!_codeWindow) {
        _codeWindow = new CodeEditorWindow(schema(), windowTitle());
        _codeWindow->setCode(function()->code());
        connect(_codeWindow, &CodeEditorWindow::closing, this, [this]{ function()->setCode(_codeWindow->code()); });
    }
    MessageBus::instance().send(MBE_MDI_CHILD_REQUESTED, {{ "wnd", QVariant::fromValue(_codeWindow.data()) }});
}
