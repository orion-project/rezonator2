#include "CodeEditorWindow.h"

#include "../app/Appearance.h"
#include "../funcs/FuncWindowHelpers.h"

#include "helpers/OriWidgets.h"
#include "tools/OriHighlighter.h"
#include "widgets/OriCodeEditor.h"

#include <QSplitter>
#include <QToolButton>

CodeEditorWindow::CodeEditorWindow(Schema *owner, const QString &title) : SchemaMdiChild(owner)
{
    setWindowIcon(QIcon(":/toolbar/python_framed"));

    _defaultTitle = !title.isEmpty() ? title : FuncWindowHelpers::makeWindowTitle("code", tr("Custom Code"));

    _editor = new Ori::Widgets::CodeEditor;
    _editor->setFont(Z::Gui::CodeEditorFont().get());
    _editor->setShowWhitespaces(true);
    _editor->setTabStopDistance(24);
    Ori::Highlighter::setHighlighter(_editor, ":/syntax/py");
    connect(_editor->document(), &QTextDocument::modificationChanged, this, &CodeEditorWindow::markModified);

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
    
    #undef A_
}

void CodeEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Code"), this, { _actnRun, nullptr, _actnClearLog });
}

void CodeEditorWindow::createToolBar()
{
    populateToolbar({
        Ori::Gui::textToolButton(_actnRun), nullptr, _actnClearLog
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

void CodeEditorWindow::logInfo(const QString &msg, bool scrollToEnd)
{
    _log->appendPlainText(msg);

    // Format the last paragraph
    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(QColor(0xff222244));
    cursor.mergeCharFormat(format);
    
    if (scrollToEnd)
        logScrollToEnd();
}

void CodeEditorWindow::logError(const QString &msg, bool scrollToEnd)
{
    _log->appendPlainText(msg);
    
    // Format the last paragraph
    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(Qt::red);
    cursor.mergeCharFormat(format);
    
    if (scrollToEnd)
        logScrollToEnd();
}

void CodeEditorWindow::logError(const QStringList &log, int errorLine)
{
    if (log.isEmpty()) return;

    for (const auto &line : log)
        logError(line, false);
    logScrollToEnd();
    
    if (errorLine > 0)
        _editor->setLineHints({{ errorLine, log.last() }});
}

void CodeEditorWindow::logScrollToEnd()
{
    auto cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    _log->setTextCursor(cursor);
    _log->ensureCursorVisible();
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
    logError("Code execution is not implemented in this window");
}

void CodeEditorWindow::updateWindowTitle()
{
    setWindowTitle(_customTitle.isEmpty() ? _defaultTitle : _customTitle);
}
