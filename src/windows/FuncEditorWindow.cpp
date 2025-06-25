#include "FuncEditorWindow.h"

#include "../app/Appearance.h"
#include "../core/PyHelper.h"

#include "helpers/OriWidgets.h"

#include <QAction>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextBlock>
#include <QToolButton>
#include <QVBoxLayout>

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
    setTitleAndIcon(tr("Custom Function"), ":/toolbar/protocol");

    _editor = new QPlainTextEdit;
    _editor->setFont(Z::Gui::CodeEditorFont().get());
    _editor->setPlaceholderText(tr("Enter function code here..."));

    _log = new QPlainTextEdit;
    _log->setFont(Z::Gui::CodeEditorFont().get());
    _log->setReadOnly(true);
    _log->setUndoRedoEnabled(false);
    _log->setPlaceholderText(tr("Execution log will appear here..."));

    createActions();
    createMenuBar();
    createToolBar();

    connect(_editor, &QPlainTextEdit::copyAvailable, _actionCut, &QAction::setEnabled);
    connect(_editor, &QPlainTextEdit::copyAvailable, _actionCopy, &QAction::setEnabled);
    connect(_editor->document(), &QTextDocument::modificationChanged, this, &FuncEditorWindow::markModified);
    connect(_editor->document(), &QTextDocument::undoAvailable, _actionUndo, &QAction::setEnabled);
    connect(_editor->document(), &QTextDocument::redoAvailable, _actionRedo, &QAction::setEnabled);

    setContent(Ori::Gui::splitterV(_editor, _log, 200, 100));
}

FuncEditorWindow::~FuncEditorWindow()
{
}

void FuncEditorWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actionUndo = A_(tr("Undo"), _editor, &QPlainTextEdit::undo, ":/toolbar/undo");
    _actionRedo = A_(tr("Redo"), _editor, &QPlainTextEdit::redo, ":/toolbar/redo");
    _actionCut = A_(tr("Cut"), _editor, &QPlainTextEdit::cut, ":/toolbar/cut");
    _actionCopy = A_(tr("Copy"), _editor, &QPlainTextEdit::copy, ":/toolbar/copy");
    _actionPaste = A_(tr("Paste"), _editor, &QPlainTextEdit::paste, ":/toolbar/paste");

    _actnRun = A_(tr("Run"), this, &FuncEditorWindow::run, ":/toolbar/start", Qt::Key_F9);
        
    #undef A_
}

void FuncEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Function"), this, { _actnRun });
}

void FuncEditorWindow::createToolBar()
{
    populateToolbar({
        Ori::Gui::textToolButton(_actnRun), nullptr,
        _actionUndo, _actionRedo, nullptr,
        _actionCut, _actionCopy, _actionPaste, nullptr,
    });
}

bool FuncEditorWindow::canUndo() { return _actionUndo->isEnabled(); }
bool FuncEditorWindow::canRedo() { return _actionRedo->isEnabled(); }
bool FuncEditorWindow::canCut() { return _actionCut->isEnabled(); }
bool FuncEditorWindow::canCopy() { return _actionCopy->isEnabled(); }
bool FuncEditorWindow::canPaste() { return _editor->canPaste(); }
void FuncEditorWindow::undo() { _actionUndo->trigger(); }
void FuncEditorWindow::redo() { _actionRedo->trigger(); }
void FuncEditorWindow::cut() { _actionCut->trigger(); }
void FuncEditorWindow::copy() { _actionCopy->trigger(); }
void FuncEditorWindow::paste() { _actionPaste->trigger(); }
void FuncEditorWindow::selectAll() { _editor->selectAll(); }

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

    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    _log->setTextCursor(cursor);
    _log->ensureCursorVisible();
}

void FuncEditorWindow::logError(const QString &msg)
{
    _log->appendPlainText(msg);
    
    // Get the last paragraph (the one we just added)
    QTextCursor cursor = _log->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    
    QTextCharFormat format;
    format.setForeground(Qt::red);
    cursor.mergeCharFormat(format);
    
    // Scroll to the end
    _log->setTextCursor(cursor);
    _log->ensureCursorVisible();
}

bool FuncEditorWindow::storableRead(const QJsonObject& root, Z::Report*)
{
    _editor->setPlainText(root["code"].toString());
    return true;
}

bool FuncEditorWindow::storableWrite(QJsonObject& root, Z::Report*)
{
    root["code"] = _editor->toPlainText();
    return true;
}

void FuncEditorWindow::run()
{
    _log->clear();

    PyHelper py;
    py.log.info = [this](const QString& msg){
        _log->appendPlainText(msg);
    };
    py.log.error = [this](const QString& msg){
        logError(msg);
    };
    //py.foo3(schema());
    
    QString code = _editor->toPlainText();
    py.foo4(schema(), code);
}
