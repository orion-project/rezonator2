#include "CustomCodeWindow.h"

#include "../core/PyRunner.h"
#include "../math/CustomFuncUtils.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriCodeEditor.h"

#include <QJsonObject>

#define FUNC_CALC QStringLiteral("calculate")
#define HELP_TOPIC "custom_script"

namespace CustomCodeWindowStorable
{
    SchemaWindow* createWindow(Schema* schema)
    {
        return CustomCodeWindow::create(schema);
    }
} // namespace CustomCodeWindowStorable

static int __funcCount = 0;

//------------------------------------------------------------------------------
//                              CustomCodeWindow
//------------------------------------------------------------------------------

CustomCodeWindow* CustomCodeWindow::create(Schema* owner, const QString &codeTemplate)
{
    auto w = new CustomCodeWindow(owner);

    if (!codeTemplate.isEmpty())
        w->_editor->loadCode(CodeUtils::codeTemplateFile(codeTemplate));
        
    return w;
}

CustomCodeWindow::CustomCodeWindow(Schema *owner) : CodeEditorWindow(owner)
{
    _moduleName = QString("customcode%1").arg(++__funcCount);
    
    connect(_editor, &QPlainTextEdit::modificationChanged, this, &CustomCodeWindow::modificationChanged);
}

void CustomCodeWindow::closeEvent(QCloseEvent* ce)
{
    if (_editor->toPlainText().trimmed().isEmpty() ||
        Ori::Dlg::ok(tr("Custom function code will be lost if you close the window")))
        SchemaMdiChild::closeEvent(ce);
    else
        ce->ignore();
}

bool CustomCodeWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    _customTitle = root["title"].toString();
    
    updateWindowTitle();
    
    return true;
}

bool CustomCodeWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    root["title"] = _customTitle;
    return true;
}

void CustomCodeWindow::runCode()
{
    PyRunner py;
    py.schema = schema();
    py.code = _editor->toPlainText();
    py.funcNames = { FUNC_CALC };
    py.funcNamesOptional = { CustomFuncUtils::funcNameMeta() };
    py.moduleName = _moduleName;
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
    
    if (!_helpTopic)
        _helpTopic = CustomFuncUtils::helpTopic(&py, HELP_TOPIC);
}

QString CustomCodeWindow::helpTopic() const
{
    if (_helpTopic)
        return *_helpTopic;
    return CustomFuncUtils::helpTopic(const_cast<CustomCodeWindow*>(this)->schema(), _editor->toPlainText(), _moduleName, HELP_TOPIC);
}

void CustomCodeWindow::modificationChanged(bool modified)
{
    if (modified && !schema()->modified())
        schema()->markModified("Custom code changed");
}
