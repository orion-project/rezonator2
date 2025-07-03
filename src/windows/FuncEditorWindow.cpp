#include "FuncEditorWindow.h"

#include "../core/PyRunner.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriCodeEditor.h"

#include <QJsonObject>

#define FUNC_CALC QStringLiteral("calculate")

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
        Ori::Dlg::ok(tr("Custom function code will be lost if you close the window")))
        SchemaMdiChild::closeEvent(ce);
    else
        ce->ignore();
}

bool FuncEditorWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    _customTitle = root["title"].toString();
    
    updateWindowTitle();
    
    return true;
}

bool FuncEditorWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    root["title"] = _customTitle;
    return true;
}

void FuncEditorWindow::runCode()
{
    PyRunner py;
    py.schema = schema();
    py.code = _editor->toPlainText();
    py.funcNames = { FUNC_CALC };
    py.printFunc = [this](const QString& s){ logInfo(s); };
    
    if (!py.load()) {
        logError(py.errorLog, py.errorLine);
        return;
    }
    
    if (py.codeTitle != _customTitle) {
        _customTitle = py.codeTitle;
        updateWindowTitle();
    }

    if (!py.run(FUNC_CALC, {}, {})) {
        logError(py.errorLog, py.errorLine);
        return;
    }
}
