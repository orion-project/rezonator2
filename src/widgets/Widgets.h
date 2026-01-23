#ifndef Z_WIDGETS_H
#define Z_WIDGETS_H

#include <QString>

namespace Ori::Widgets {
class CodeEditor;
}

class QLineEdit;
class QObject;
class QPlainTextEdit;
class QTextEdit;
class QToolBar;

namespace Z::Gui {

/// Make code editor for custom scripts
/// so that is looks similar in all places where it's used.
Ori::Widgets::CodeEditor* makeCodeEditor();

/// Make toolbar for using in top-level windows
/// so that is looks similar in all places where it's used.
QToolBar* makeToolBar(std::initializer_list<QObject*> items);

QLineEdit* makeFilterEdit(const QString &placeholder, QObject *receiver, const char* applyFilterSlot);

void addLogInfo(QTextEdit *logView, const QString &msg, bool scrollToEnd = true);
void addLogInfo(QPlainTextEdit *logView, const QString &msg, bool scrollToEnd = true);
void addLogError(QTextEdit *logView, const QString &msg, bool scrollToEnd = true);
void addLogError(QPlainTextEdit *logView, const QString &msg, bool scrollToEnd = true);
void scrollToEnd(QTextEdit *logView);
void scrollToEnd(QPlainTextEdit *logView);

}

#endif // Z_WIDGETS_H