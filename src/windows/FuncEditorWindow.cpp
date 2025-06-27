#include "FuncEditorWindow.h"


#include "../core/PyRunner.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriCodeEditor.h"

#include <QJsonObject>

namespace FuncEditorWindowStorable
{
    SchemaWindow* createWindow(Schema* schema)
    {
        return FuncEditorWindow::create(schema);
    }
} // namespace FuncEditorWindowStorable

//------------------------------------------------------------------------------
//                              FuncEditorWindow
//------------------------------------------------------------------------------

FuncEditorWindow* FuncEditorWindow::create(Schema* owner)
{
    return new FuncEditorWindow(owner);
}

FuncEditorWindow::FuncEditorWindow(Schema *owner) : CodeEditorWindow(owner)
{
}

void FuncEditorWindow::closeEvent(QCloseEvent* ce)
{
    if (_editor->toPlainText().trimmed().isEmpty() ||
        Ori::Dlg::ok(tr("Function code will be lost if you close the window")))
        SchemaMdiChild::closeEvent(ce);
    else
        ce->ignore();
}

bool FuncEditorWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    _funcTitle = root["title"].toString();
    
    updateWindowTitle();
    
    return true;
}

bool FuncEditorWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    root["title"] = _funcTitle;
    return true;
}

void FuncEditorWindow::runCode()
{
    QString funcName("calc");

    PyRunner py;
    py.logInfo = [this](const QString& msg){ logInfo(msg); };
    py.logError = [this](const QString& msg){ logError(msg); };
    py.schema = schema();
    py.code = _editor->toPlainText();
    py.moduleName = _moduleName;
    py.funcNames = { funcName };
    
    if (!py.load())
        return;
        
    if (py.funcTitles[funcName] != _funcTitle) {
        _funcTitle = py.funcTitles[funcName];
        updateWindowTitle();
    }

    if (!py.run(funcName))
        return;
}
