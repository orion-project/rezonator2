#include "ElemFormulaEditor.h"

#include "../app/Appearance.h"
#include "../core/ElementFormula.h"
#include "../math/FormatInfo.h"
#include "../widgets/UnitWidgets.h"
#include "../widgets/Widgets.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriCodeEditor.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QPlainTextEdit>

using Me = ElemFormulaEditor;

ElemFormulaEditor::ElemFormulaEditor(ElemFormula* element): QWidget(), _element(element)
{
    createActions();
    createToolbar();

    _codeEditor = Z::Gui::makeCodeEditor();
    _codeEditor->setCode(_element->formula());
    connect(_codeEditor, &QPlainTextEdit::modificationChanged, this, &Me::codeModified);

    _logView = new QTextEdit;
    _logView->setReadOnly(true);
    _logView->setAcceptRichText(false);
    _logView->setFont(Z::Gui::CodeEditorFont().get());
    _logView->document()->setDefaultStyleSheet(Z::Gui::reportStyleSheet());

    auto codeSplitter = Ori::Gui::splitterV(_codeEditor, 80, _logView, 20);
    Ori::Layouts::LayoutV({_toolbar, codeSplitter}).setMargin(0).useFor(this);
}

void ElemFormulaEditor::createActions()
{
    #define A_ Ori::Gui::action
    _actnApplyCode = A_(tr("Apply Formula"), this, &Me::applyFormula, ":/toolbar/check", "Ctrl+B");
    _actnClearLog = A_(tr("Clear Log"), this, &Me::clearLog, ":/toolbar/clear_log");
    _actnShowHelp = A_(tr("Help"), this, &Me::showHelp, ":/toolbar/help");
    #undef A_
    
    _actnApplyCode->setIconText(tr("Apply"));
}

void ElemFormulaEditor::createToolbar()
{
    _toolbar = Z::Gui::makeToolBar({
        Ori::Gui::textToolButton(_actnApplyCode), _actnClearLog, 0,
        _actnShowHelp,
    });
}

void ElemFormulaEditor::populateWindowMenu(QMenu* menu)
{
    menu->addAction(_actnApplyCode);
    menu->addAction(_actnClearLog);
}

void ElemFormulaEditor::resetModifiedFlag()
{
    _lockEvents = true;
    _codeEditor->document()->setModified(false);
    _lockEvents = false;
}

QString ElemFormulaEditor::code() const
{
    return _codeEditor->code();
}

void ElemFormulaEditor::setCode(const QString &code)
{
    _lockEvents = true;
    _isChanged = true;
    _codeEditor->setCode(code);
    _lockEvents = false;
}

void ElemFormulaEditor::codeModified()
{
    if (_lockEvents) return;
    _isChanged = true;
    emit onChange();
}

void ElemFormulaEditor::applyFormula()
{
    clearLog();
    
    ElemFormula elem;
    for (auto param : _element->params())
    {
        auto paramCopy = new Z::Parameter;
        paramCopy->copyFrom(param);
        elem.addParam(paramCopy);
    }
    elem.setFormula(_codeEditor->code());
    elem.setPrintFunc([this](const QString &s){ Z::Gui::addLogInfo(_logView, s); });
    elem.calcMatrix("ElemFormulaEditor::applyFormula");
    if (elem.failed())
    {
        auto log = elem.errorLog();
        if (!log.isEmpty())
        {
            int errorLine = elem.errorLine();
            qDebug() << "Script error at line" << errorLine << ':' << log;
        
            for (const auto &line : std::as_const(log))
                Z::Gui::addLogError(_logView, line, false);
            Z::Gui::scrollToEnd(_logView);
            
            if (errorLine > 0)
                _codeEditor->setLineHints({{ errorLine, log.last() }});
        }
        return;
    }

    Z::Gui::scrollToEnd(_logView);
    _logView->insertHtml(Z::Format::matrices(elem.Mt(), elem.Ms()));
    Z::Gui::scrollToEnd(_logView);
    
    _element->assign(&elem);
    _isChanged = false;
    resetModifiedFlag();
    emit onChange();
    emit onApply();
}

void ElemFormulaEditor::clearLog()
{
    _logView->clear();
    _codeEditor->setLineHints({});
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
