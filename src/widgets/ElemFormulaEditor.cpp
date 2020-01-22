#include "ElemFormulaEditor.h"

#include "ParamEditor.h"
#include "ParamsEditor.h"
#include "UnitWidgets.h"
#include "../Appearance.h"
#include "../WindowsManager.h"
#include "../core/ElementFormula.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriFlatToolBar.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QToolButton>
#include <QPlainTextEdit>

ElemFormulaEditor::ElemFormulaEditor(ElemFormula* elem, bool fullToolbar) : QWidget(), _element(elem)
{
    createActions();
    createToolbar(fullToolbar);

    ParamsEditor::Options opts(&_parameters);
    opts.menuButtonActions = {_actnParamDescr, nullptr, _actnParamDelete};
    _paramsEditor = new ParamsEditor(opts);

    _flagHasTandSMatrices = new QCheckBox(tr("Different matrices for T and S"));

    auto paramsPanel = Ori::Layouts::LayoutV({
        Z::Gui::headerlabel(tr(" Options")),
        Ori::Layouts::LayoutV({_flagHasTandSMatrices}).setMargin(6),
        Ori::Layouts::Space(6),
        Z::Gui::headerlabel(tr(" Parameters")),
        _paramsEditor,
    }).setMargin(0).makeWidget();

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
    mainSplitter->setStretchFactor(0, 10);
    mainSplitter->setStretchFactor(1, 90);

    Ori::Layouts::LayoutV({_toolbar, mainSplitter}).setMargin(0).useFor(this);
}

void ElemFormulaEditor::createActions()
{
    #define A_ Ori::Gui::action

    _actnSaveChanges = A_(tr("Save Changes"), this, SLOT(saveChanges()), ":/toolbar/elem_arrow_to");
    _actnResetChanges = A_(tr("Reset Changes"), this, SLOT(resetChanges()), ":/toolbar/elem_arrow_from");
    _actnCheckCode = A_(tr("Check Formula"), this, SLOT(checkFormula()), ":/toolbar/check", Qt::CTRL | Qt::Key_B);
    _actnClearLog = A_(tr("Clear Log"), this, SLOT(clearLog()), ":/toolbar/clear_log");
    _actnShowHelp = A_(tr("Help"), this, SLOT(showHelp()), ":/toolbar/help");
    _actnParamAdd = A_(tr("Add Parameter..."), this, SLOT(createParameter()), ":/toolbar/param_add");
    _actnParamDelete = A_(tr("Delete..."), this, SLOT(deleteParameter()), ":/toolbar/param_delete");
    _actnParamDescr = A_(tr("Annotate..."), this, SLOT(annotateParameter()), ":/toolbar/param_annotate");

    #undef A_
}

void ElemFormulaEditor::createToolbar(bool full)
{
    _toolbar = new Ori::Widgets::FlatToolBar;
    _toolbar->setIconSize(Z::WindowUtils::toolbarIconSize());

    _toolbar->addAction(_actnSaveChanges);
    _toolbar->addAction(_actnResetChanges);
    _toolbar->addSeparator();
    _toolbar->addAction(_actnParamAdd);
    _toolbar->addSeparator();
    _toolbar->addWidget(Ori::Gui::textToolButton(_actnCheckCode));

    if (full)
    {
        _toolbar->addAction(_actnClearLog);
        _toolbar->addSeparator();
        _toolbar->addAction(_actnShowHelp);
    }
}

void ElemFormulaEditor::populateWindowMenu(QMenu* menu)
{
    menu->addAction(_actnSaveChanges);
    menu->addAction(_actnResetChanges);
    menu->addSeparator();
    menu->addAction(_actnCheckCode);
    menu->addAction(_actnClearLog);
    menu->addSeparator();
    menu->addAction(_actnParamAdd);
}

void ElemFormulaEditor::saveChanges()
{
    // TODO
    _logView->append("Save changes");
}

void ElemFormulaEditor::resetChanges()
{
    // TODO
    _logView->append("Reset changes");
}

void ElemFormulaEditor::checkFormula()
{
    // TODO
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

bool ElemFormulaEditor::canCopy()
{
    return _codeEditor->hasFocus() || _logView->hasFocus();
}

bool ElemFormulaEditor::canPaste()
{
    if (_codeEditor->hasFocus())
        return _codeEditor->canPaste();
    else if (_logView->hasFocus())
        return _logView->canPaste();
    return false;
}

void ElemFormulaEditor::selectAll()
{
    if (_codeEditor->hasFocus())
        _codeEditor->selectAll();
    else if (_logView->hasFocus())
        _logView->selectAll();
}

void ElemFormulaEditor::copy()
{
    if (_codeEditor->hasFocus())
        _codeEditor->copy();
    else if (_logView->hasFocus())
        _logView->copy();
}

void ElemFormulaEditor::paste()
{
    if (_codeEditor->hasFocus())
        _codeEditor->paste();
    else if (_logView->hasFocus())
        _logView->paste();
}

void ElemFormulaEditor::createParameter()
{
    auto aliasEditor = new QLineEdit;
    aliasEditor->setFont(Z::Gui::ValueFont().get());

    auto dimEditor = new DimComboBox;
    dimEditor->setSelectedDim(Z::Dims::none());

    QWidget editor;
    auto layout = new QFormLayout(&editor);
    layout->setMargin(0);
    layout->addRow(new QLabel(tr("Name")), aliasEditor);
    layout->addRow(new QLabel(tr("Dim")), dimEditor);

    auto verifyFunc = [&](){
        auto alias = aliasEditor->text().trimmed();
        if (alias.isEmpty())
            return tr("Parameter name can't be empty");
        if (_parameters.byAlias(alias))
            return tr("Parameter '%1' already exists").arg(alias);
        if (!Z::FormulaUtils::isValidVariableName(alias))
            return tr("Parameter name '%1' is invalid").arg(alias);
        return QString();
    };

    if (Ori::Dlg::Dialog(&editor, false)
                .withTitle(tr("Create Parameter"))
                .withIconPath(":/window_icons/parameter")
                .withContentToButtonsSpacingFactor(3)
                .withVerification(verifyFunc)
                .exec())
    {
        auto dim = dimEditor->selectedDim();
        auto unit = dim->units().first();
        auto alias = aliasEditor->text().trimmed();
        auto label = alias;
        auto name = alias;
        auto param = new Z::Parameter(dim, alias, label, name);
        param->setValue(Z::Value(0, unit));
        _parameters.append(param);
        _paramsEditor->addEditor(param);
        _paramsEditor->populateValues();
        _paramsEditor->focus(param);
    }
}

void ElemFormulaEditor::deleteParameter()
{
    qDebug() << "Annontate" << _actnParamDelete->data().value<ParamEditor*>()->parameter()->displayStr();
}

void ElemFormulaEditor::annotateParameter()
{
    qDebug() << "Annontate" << _actnParamDescr->data().value<ParamEditor*>()->parameter()->displayStr();
}
