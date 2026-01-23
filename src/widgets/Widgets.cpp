#include "../app/Appearance.h"

#include "helpers/OriWidgets.h"
#include "widgets/OriCodeEditor.h"
#include "widgets/OriFlatToolBar.h"
#include "tools/OriHighlighter.h"

#include <QDebug>
#include <QLineEdit>

namespace Z::Gui {

Ori::Widgets::CodeEditor* makeCodeEditor()
{
    auto ed = new Ori::Widgets::CodeEditor;
    ed->setFont(Z::Gui::CodeEditorFont().get());
    ed->setShowWhitespaces(true);
    ed->setTabWidth(2);
    ed->setBackgroundVisible(true);
    Ori::Highlighter::setHighlighter(ed, ":/syntax/py");
    return ed;
}

QToolBar* makeToolBar(std::initializer_list<QObject*> items)
{
    auto tb = new Ori::Widgets::FlatToolBar;
    tb->setIconSize(Z::Gui::toolbarIconSize());
    
    Ori::Gui::populate(tb, items);
    
    return tb;
}

QLineEdit* makeFilterEdit(const QString &placeholder, QObject *receiver, const char* applyFilterSlot)
{
    auto ed = new QLineEdit;
    ed->setPlaceholderText(placeholder);
    ed->setClearButtonEnabled(true);
    ed->connect(ed, SIGNAL(textChanged(QString)), receiver, applyFilterSlot);
    auto a  = new QAction(ed);
    a->connect(a, &QAction::triggered, ed, [ed]{ ed->setFocus(); });
    a->setShortcut(Qt::Key_F3);
    ed->addAction(a);
    return ed;
}

template <class TLogView>
void _scrollToEnd(TLogView *logView)
{
    auto cursor = logView->textCursor();
    cursor.movePosition(QTextCursor::End);
    logView->setTextCursor(cursor);
    logView->ensureCursorVisible();
}

void scrollToEnd(QTextEdit *logView) { _scrollToEnd(logView); }
void scrollToEnd(QPlainTextEdit *logView) { _scrollToEnd(logView); }

template <class TLogView>
void _processLastPara(TLogView *logView, bool error, bool scrollToEnd)
{
    // Format the last paragraph
    QTextCursor cursor = logView->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(error ? Qt::red : QColor(0xff222244));
    cursor.mergeCharFormat(format);
    
    if (scrollToEnd)
        _scrollToEnd(logView);
}

void addLogInfo(QTextEdit *logView, const QString &msg, bool scrollToEnd)
{
    qDebug() << msg;
    logView->append(msg);
    _processLastPara(logView, false, scrollToEnd);
}

void addLogInfo(QPlainTextEdit *logView, const QString &msg, bool scrollToEnd)
{
    logView->appendPlainText(msg);
    _processLastPara(logView, false, scrollToEnd);
}

void addLogError(QTextEdit *logView, const QString &msg, bool scrollToEnd)
{
    logView->append(msg);
    _processLastPara(logView, true, scrollToEnd);
}

void addLogError(QPlainTextEdit *logView, const QString &msg, bool scrollToEnd)
{
    logView->appendPlainText(msg);
    _processLastPara(logView, true, scrollToEnd);
}

} // namespace Z::Gui
