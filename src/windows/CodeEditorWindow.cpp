#include "CodeEditorWindow.h"

#include "../app/Appearance.h"
#include "../app/CustomFuncsLib.h"
#include "../funcs/FuncWindowHelpers.h"
#include "../widgets/Widgets.h"

#include "helpers/OriWidgets.h"
#include "widgets/OriCodeEditor.h"

#include <QFile>
#include <QSplitter>
#include <QToolButton>

CodeEditorWindow::CodeEditorWindow(Schema *owner, const QString &title) : SchemaMdiChild(owner)
{
    setWindowIcon(QIcon(":/toolbar/python_framed"));

    _defaultTitle = !title.isEmpty() ? title : FuncWindowHelpers::makeWindowTitle("code", tr("Custom Script"));

    _editor = Z::Gui::makeCodeEditor();

    _log = new QPlainTextEdit;
    _log->setFont(Z::Gui::CodeEditorFont().get());
    _log->setReadOnly(true);
    _log->setUndoRedoEnabled(false);
    _log->setTabStopDistance(24);
    
    createActions();
    createMenuBar();
    createToolBar();
    updateWindowTitle();

    setContent(Ori::Gui::splitterV(_editor, _log, 200, 100));
}

void CodeEditorWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnRun = A_(tr("Run"), this, &CodeEditorWindow::run, ":/toolbar/start", Qt::Key_F9);
    _actnClearLog = A_(tr("Clear Log"), this, &CodeEditorWindow::clearLog, ":/toolbar/clear_log");
    _actnSaveCustom = A_(tr("Save to Custom Library..."), this, &CodeEditorWindow::saveToLibrary, ":/toolbar/book_arrow_down");
    
    #undef A_
}

void CodeEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Script"), this, { _actnRun, nullptr, _actnClearLog, nullptr, _actnSaveCustom });
}

void CodeEditorWindow::createToolBar()
{
    populateToolbar({
        Ori::Gui::textToolButton(_actnRun), nullptr, _actnClearLog, nullptr, _actnSaveCustom
    });
}

bool CodeEditorWindow::canUndo() { return _editor->hasFocus() && _editor->document()->isUndoAvailable(); }
bool CodeEditorWindow::canRedo() { return _editor->hasFocus() && _editor->document()->isRedoAvailable(); }
bool CodeEditorWindow::canCut() { return _editor->hasFocus(); }
bool CodeEditorWindow::canCopy() { return true; }
bool CodeEditorWindow::canPaste() { return _editor->hasFocus() && _editor->canPaste(); }
void CodeEditorWindow::undo() { if (_editor->hasFocus()) _editor->undo(); }
void CodeEditorWindow::redo() { if (_editor->hasFocus()) _editor->redo(); }
void CodeEditorWindow::cut() { if (_editor->hasFocus()) _editor->cut(); }
void CodeEditorWindow::copy() { if (_log->hasFocus()) _log->copy(); else _editor->copy(); }
void CodeEditorWindow::paste() { if (_editor->hasFocus()) _editor->paste(); }
void CodeEditorWindow::selectAll() { if (_log->hasFocus()) _log->selectAll(); else _editor->selectAll(); }

void CodeEditorWindow::markModified(bool m)
{
    if (m && !_isChanging)
        schema()->events().raise(SchemaEvents::Changed, "custom code changed");
}

void CodeEditorWindow::schemaSaved(Schema*)
{
    _editor->document()->setModified(false);
}

QString CodeEditorWindow::code() const
{
    return _editor->toPlainText();
}

void CodeEditorWindow::setCode(const QString &code)
{
    _isChanging = true;
    clearLog();
    _editor->setPlainText(code);
    _editor->setLineHints({});
    _isChanging = false;
}

void CodeEditorWindow::logError(const QStringList &log, int errorLine)
{
    if (log.isEmpty()) return;
    
    qDebug() << "Script error at line" << errorLine << ':' << log;

    for (const auto &line : log)
        Z::Gui::addLogError(_log, line, false);
    Z::Gui::scrollToEnd(_log);
    
    if (errorLine > 0)
        _editor->setLineHints({{ errorLine, log.last() }});
}

void CodeEditorWindow::logInfo(const QString &msg)
{
    Z::Gui::addLogInfo(_log, msg);
}

void CodeEditorWindow::clearLog()
{
    _log->clear();
    _editor->setLineHints({});
}

void CodeEditorWindow::run()
{
    clearLog();
    runCode();
}

void CodeEditorWindow::runCode()
{
    Z::Gui::addLogError(_log, "Code execution is not implemented in this window");
}

void CodeEditorWindow::updateWindowTitle()
{
    setWindowTitle(_customTitle.isEmpty() ? _defaultTitle : _customTitle);
}

void CodeEditorWindow::saveToLibrary()
{
    CustomFuncsLib::put(code());
}

//------------------------------------------------------------------------------
//                                 CodeUtils
//------------------------------------------------------------------------------

namespace CodeUtils
{

QString codeTemplateFile(const QString &templateName)
{
    return qApp->applicationDirPath() + "/functions/" + templateName + ".py";
}

QString loadCodeTemplate(const QString &templateName)
{
    QString fileName = codeTemplateFile(templateName);
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning() << "Failed to open" << fileName << f.errorString();
        return {};
    }
    return QString::fromUtf8(f.readAll());
}

} // namespace CodeUtils

