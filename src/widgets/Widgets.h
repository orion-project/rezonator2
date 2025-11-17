#ifndef Z_WIDGETS_H
#define Z_WIDGETS_H

#include <QString>

namespace Ori::Widgets {
class CodeEditor;
}

class QLineEdit;
class QObject;
class QToolBar;

namespace Z::Gui {

/// Make code editor for custom scripts
/// so that is looks similar in all places where it's used.
Ori::Widgets::CodeEditor* makeCodeEditor();

/// Make toolbar for using in top-level windows
/// so that is looks similar in all places where it's used.
QToolBar* makeToolBar(std::initializer_list<QObject*> items);

QLineEdit* makeFilterEdit(const QString &placeholder, QObject *receiver, const char* applyFilterSlot);
}

#endif // Z_WIDGETS_H