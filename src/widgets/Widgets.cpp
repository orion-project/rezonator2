#include "../app/Appearance.h"

#include "helpers/OriWidgets.h"
#include "widgets/OriCodeEditor.h"
#include "widgets/OriFlatToolBar.h"
#include "tools/OriHighlighter.h"

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

} // namespace Z::Gui
