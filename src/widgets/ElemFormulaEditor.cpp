#include "ElemFormulaEditor.h"

#include "../Appearance.h"
#include "../WindowsManager.h"
#include "../core/ElementFormula.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriFlatToolBar.h"

#include <QSplitter>
#include <QToolButton>
#include <QPlainTextEdit>

bool ElemFormulaEditor::editFormula(ElemFormula* elem)
{
    ElemFormulaEditor editor(elem);

    return Ori::Dlg::Dialog(&editor, false)
            .withTitle(tr("Edit Element Formula"))
            .withContentToButtonsSpacingFactor(2)
            .withStretchedContent()
            .withInitialSize({800, 400})
            .exec();
}

ElemFormulaEditor::ElemFormulaEditor(ElemFormula* elem, QWidget *parent) : QWidget(parent), _elem(elem)
{
    auto paramsPanel = new QWidget;

    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setFont(Z::Gui::CodeEditorFont().get());

    _logView = new QTextEdit;
    _logView->setReadOnly(true);
    _logView->setAcceptRichText(false);
    _logView->setFont(Z::Gui::CodeEditorFont().get());

    auto codeSplitter = Ori::Gui::splitterV(_codeEditor, _logView);
    codeSplitter->setStretchFactor(0, 80);
    codeSplitter->setStretchFactor(1, 20);

    auto mainSplitter = Ori::Gui::splitterH(paramsPanel, codeSplitter);
    mainSplitter->setStretchFactor(0, 20);
    mainSplitter->setStretchFactor(1, 80);

    createActions();
    createToolbar();

    Ori::Layouts::LayoutV({_toolbar, mainSplitter}).setMargin(0).useFor(this);
}

void ElemFormulaEditor::createActions()
{
    #define A_ Ori::Gui::action

    _actnCheck = A_(tr("Check Formula"), this, SLOT(checkFormula()), ":/toolbar/check", Qt::CTRL | Qt::Key_B);
    _actnClearLog = A_(tr("Clear Log"), this, SLOT(clearLog()), ":/toolbar/clear_log");
    _actnHelp = A_(tr("Help"), this, SLOT(showHelp()), ":/toolbar/help");

    #undef A_
}

void ElemFormulaEditor::createToolbar()
{
    _toolbar = new Ori::Widgets::FlatToolBar;
    _toolbar->setIconSize(Z::WindowUtils::toolbarIconSize());

    Ori::Gui::populate(_toolbar, { Ori::Gui::textToolButton(_actnCheck), nullptr, _actnClearLog, nullptr, _actnHelp });
}

void ElemFormulaEditor::checkFormula()
{
    _logView->append("Check formula");
}

void ElemFormulaEditor::clearLog()
{
    _logView->clear();
}

void ElemFormulaEditor::showHelp()
{
    // TODO
    _logView->append("Show help");
}
