#include "FuncEditorWindow.h"

#include "../app/Appearance.h"

#include "helpers/OriWidgets.h"

#include <QAction>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QSplitter>
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

    createActions();
    createMenuBar();
    createToolBar();
    createContent();
}

FuncEditorWindow::~FuncEditorWindow()
{
}

void FuncEditorWindow::createContent()
{
    _editor = new QPlainTextEdit;
    _editor->setFont(Z::Gui::CodeEditorFont().get());
    _editor->setPlaceholderText(tr("Enter function code here..."));
    
    _log = new QPlainTextEdit;
    _log->setFont(Z::Gui::CodeEditorFont().get());
    _log->setReadOnly(true);
    _log->setPlaceholderText(tr("Execution log will appear here..."));
    
    setContent(Ori::Gui::splitterV(_editor, _log, 200, 100));
}

void FuncEditorWindow::createActions()
{
    #define A_ Ori::Gui::action

    _actnCheck = A_(tr("Check"), this, &FuncEditorWindow::checkFunction, ":/toolbar/check", Qt::Key_F9);
        
    #undef A_
}

void FuncEditorWindow::createMenuBar()
{
    _windowMenu = Ori::Gui::menu(tr("Function"), this, { _actnCheck });
}

void FuncEditorWindow::createToolBar()
{
    populateToolbar({ Ori::Gui::textToolButton(_actnCheck) });
}

void FuncEditorWindow::checkFunction()
{
    _log->appendPlainText("TODO: Implement function checking");
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
