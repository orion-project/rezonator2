#include "FuncEditorWindow.h"

#include "../app/Appearance.h"
#include "../core/PyHelper.h"

#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "widgets/OriCodeEditor.h"

#include <QAction>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextBlock>
#include <QToolButton>
#include <QVBoxLayout>

#define CUSTOM_MODULE QStringLiteral("customfunc")

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

FuncEditorWindow::FuncEditorWindow(Schema *owner) : SchemaMdiChild(owner)
{
    setTitleAndIcon(tr("Custom Function Code"), ":/toolbar/protocol");

    _editor = new Ori::Widgets::CodeEditor;
    _editor->setFont(Z::Gui::CodeEditorFont().get());
    _editor->setShowWhitespaces(true);
    _editor->setTabStopDistance(24);
    Ori::Highlighter::setHighlighter(_editor, ":/syntax/py");

    _log = new QPlainTextEdit;
    _log->setFont(Z::Gui::CodeEditorFont().get());
    _log->setReadOnly(true);
    _log->setUndoRedoEnabled(false);

    createActions();
    createMenuBar();
    createToolBar();

    connect(_editor->document(), &QTextDocument::modificationChanged, this, &FuncEditorWindow::markModified);

    setContent(Ori::Gui::splitterV(_editor, _log, 200, 100));
}

FuncEditorWindow::~FuncEditorWindow()
{
}

void FuncEditorWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnRun = A_(tr("Run"), this, &FuncEditorWindow::run, ":/toolbar/start", Qt::Key_F9);
    _actnClearLog = A_(tr("Clear Log"), this, &FuncEditorWindow::clearLog, ":/toolbar/clear_log");
    
    #undef A_
}

void FuncEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Code"), this, { _actnRun, nullptr, _actnClearLog });
}

void FuncEditorWindow::createToolBar()
{
    populateToolbar({
        Ori::Gui::textToolButton(_actnRun), nullptr, _actnClearLog
    });
}

bool FuncEditorWindow::canUndo() { return _editor->hasFocus() && _editor->document()->isUndoAvailable(); }
bool FuncEditorWindow::canRedo() { return _editor->hasFocus() && _editor->document()->isRedoAvailable(); }
bool FuncEditorWindow::canCut() { return _editor->hasFocus(); }
bool FuncEditorWindow::canCopy() { return true; }
bool FuncEditorWindow::canPaste() { return _editor->hasFocus() && _editor->canPaste(); }
void FuncEditorWindow::undo() { if (_editor->hasFocus()) _editor->undo(); }
void FuncEditorWindow::redo() { if (_editor->hasFocus()) _editor->redo(); }
void FuncEditorWindow::cut() { if (_editor->hasFocus()) _editor->cut(); }
void FuncEditorWindow::copy() { if (_log->hasFocus()) _log->copy(); else _editor->copy(); }
void FuncEditorWindow::paste() { if (_editor->hasFocus()) _editor->paste(); }
void FuncEditorWindow::selectAll() { if (_log->hasFocus()) _log->selectAll(); else _editor->selectAll(); }

void FuncEditorWindow::markModified(bool m)
{
    if (m)
    {
        schema()->events().raise(SchemaEvents::Changed, "cunstom func modified");
    }
}

void FuncEditorWindow::logInfo(const QString &msg)
{
    _log->appendPlainText(msg);

    // Format the last paragraph
    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(QColor(0xff222244));
    cursor.mergeCharFormat(format);
    
    logScrollToEnd();
}

void FuncEditorWindow::logError(const QString &msg)
{
    // Parse python error of the form:
    //
    // ERROR: Failed to compile py code
    // File "customfunc.py", line 7
    //	p1 = sche ma.param("P1")
    //	          ^^
    // SyntaxError: invalid syntax
    //
    QStringList lines = msg.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : std::as_const(lines)) {
        static QRegularExpression r(R"(File \"(.+)\",\s+line\s+(\d+))");
        auto m = r.match(line);
        if (!m.hasMatch()) continue;
        if (m.captured(1) != CUSTOM_MODULE) continue;
        int lineNo = m.captured(2).toInt();
        _editor->setLineHints({{ lineNo, lines.last() }});
        break;
    }

    _log->appendPlainText(msg);
    
    // Format the last paragraph
    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(Qt::red);
    cursor.mergeCharFormat(format);
    
    logScrollToEnd();
}

void FuncEditorWindow::logScrollToEnd()
{
    auto cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    _log->setTextCursor(cursor);
    _log->ensureCursorVisible();
}

bool FuncEditorWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    _funcTitle = root["title"].toString();
    
    if (!_funcTitle.isEmpty())
        setWindowTitle(_funcTitle);
    
    return true;
}

bool FuncEditorWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    root["title"] = _funcTitle;
    return true;
}

void FuncEditorWindow::clearLog()
{
    _log->clear();
    _editor->setLineHints({});
}

void FuncEditorWindow::run()
{
    clearLog();

    PyHelper py;
    py.logInfo = [this](const QString& msg){ logInfo(msg); };
    py.logError = [this](const QString& msg){ logError(msg); };
    py.schema = schema();
    py.code = _editor->toPlainText();
    py.moduleName = CUSTOM_MODULE;
    py.funcName = "calc";
    
    py.run();
    
    if (py.funcTitle != _funcTitle) {
        _funcTitle = py.funcTitle;
        setWindowTitle(_funcTitle);
    }
}
