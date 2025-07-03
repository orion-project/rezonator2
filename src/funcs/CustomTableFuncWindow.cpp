#include "CustomTableFuncWindow.h"

#include "CustomTableCodeWindow.h"
#include "../app/MessageBus.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"

#include <QCloseEvent>
#include <QJsonObject>
#include <QMenu>
#include <QToolBar>

#define A_ Ori::Gui::action

CustomTableFuncWindow::CustomTableFuncWindow(Schema* schema): TableFuncWindow(new CustomTableFunction(schema))
{
    _defaultWindowTitle = windowTitle();

    _actnShowCode = A_(tr("Show Code"), this, &CustomTableFuncWindow::showCode, ":/toolbar/python_framed");
    
    _menuTable->addSeparator();
    _menuTable->addAction(_actnShowCode);
    
    toolbar()->addSeparator();
    toolbar()->addAction(_actnShowCode);
}

void CustomTableFuncWindow::closeEvent(QCloseEvent* ce)
{
    if (function()->code().trimmed().isEmpty() ||
        Ori::Dlg::ok(tr("Custom function code will be lost if you close the window")))
    {
        if (_codeWindow)
            _codeWindow->close();
        SchemaMdiChild::closeEvent(ce);
    }
    else
        ce->ignore();
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

void CustomTableFuncWindow::beforeUpdate()
{
    if (_codeWindow) {
        _codeWindow->clearLog();
        function()->setCode(_codeWindow->code());
    }
}

void CustomTableFuncWindow::beforeUpdateTable()
{
    _table->updateColumnCount();
    _table->updateColumnLabels();
}

void CustomTableFuncWindow::afterUpdate()
{
    QString customTitle = function()->customTitle();
    setWindowTitle(customTitle.isEmpty() ? _defaultWindowTitle : customTitle);
    if (_codeWindow) {
        _codeWindow->showResult();
        _codeWindow->setWindowTitle(windowTitle());
    }
}

void CustomTableFuncWindow::showCode()
{
    if (!_codeWindow) {
        _codeWindow = new CustomTableCodeWindow(function(), [this]{ update(); });
        _codeWindow->setWindowTitle(windowTitle());
        connect(_codeWindow, &CodeEditorWindow::closing, this, [this]{ update(); });
    }
    MessageBus::instance().send(MBE_MDI_CHILD_REQUESTED, {{ "wnd", QVariant::fromValue(_codeWindow.data()) }});
}
