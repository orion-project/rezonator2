#include "ElemFormulaEditor.h"

#include "../app/Appearance.h"
#include "../core/ElementFormula.h"
#include "../core/Formula.h"
#include "../math/FormatInfo.h"
#include "../widgets/ParamEditor.h"
#include "../widgets/ParamsEditor.h"
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

using namespace Ori::Layouts;
using Me = ElemFormulaEditor;

template <class TPayload>
class ObjectDeleter : public QObject
{
public:
    ObjectDeleter(TPayload* payload, QObject* parent) : QObject(parent), _payload(payload) {}
    ~ObjectDeleter() override { delete _payload; }
private:
    TPayload* _payload;
};


ElemFormulaEditor::ElemFormulaEditor(ElemFormula* sourceElem, ElemFormula *workingCopy)
    : QWidget(), _sourceElem(sourceElem), _workingCopy(workingCopy)
{
    if (!_workingCopy)
    {
        _workingCopy = new ElemFormula;
        _workingCopy->assign(_sourceElem);
    }

    createActions();
    createToolbar();

    ParamsEditor::Options opts;
    opts.menuButtonActions = {
        _actnParamDescr, nullptr, _actnParamMoveUp, _actnParamMoveDown, nullptr, _actnParamDelete
    };
    _paramsEditor = new ParamsEditor(_workingCopy->params(), opts);
    connect(_paramsEditor, &ParamsEditor::paramChanged, this, &Me::paramsChanged);

    _stubNoParams = LayoutV({
        LayoutH({ Stretch(), tr("Element nas no parameters"), Stretch() }),
        Stretch(),
    }).makeWidget();

    auto paramsPanel = LayoutV({
        // Z::Gui::makeHeaderLabel(tr(" Options")),
        // LayoutV({}).setMargin(6),
        // Space(6),
        Z::Gui::makeHeaderLabel(tr(" Parameters")),
        _paramsEditor,
        _stubNoParams,
    }).setMargin(0).makeWidget();

    _codeEditor = Z::Gui::makeCodeEditor();
    connect(_codeEditor, &QPlainTextEdit::modificationChanged, this, &Me::codeModified);

    _logView = new QTextEdit;
    _logView->setReadOnly(true);
    _logView->setAcceptRichText(false);
    _logView->setFont(Z::Gui::CodeEditorFont().get());
    _logView->document()->setDefaultStyleSheet(Z::Gui::reportStyleSheet());

    auto codeSplitter = Ori::Gui::splitterV(_codeEditor, 80, _logView, 20);
    auto mainSplitter = Ori::Gui::splitterH(paramsPanel, 10, codeSplitter, 90);
    Ori::Layouts::LayoutV({_toolbar, mainSplitter}).setMargin(0).useFor(this);

    // Element should be deleted only after all children, so put at the end
    new ObjectDeleter<Element>(_workingCopy, this);

    qDebug() << "Initial populate";
    populate();
}

ElemFormulaEditor::~ElemFormulaEditor()
{
}

void ElemFormulaEditor::createActions()
{
    #define A_ Ori::Gui::action
    _actnApplyChanges = A_(tr("Apply Changes"), this, &Me::collect, ":/toolbar/elem_arrow_to");
    _actnResetChanges = A_(tr("Reset Changes"), this, &Me::reset, ":/toolbar/elem_arrow_from");
    _actnCheckCode = A_(tr("Check Formula"), this, &Me::checkFormula, ":/toolbar/check", "Ctrl+B");
    _actnClearLog = A_(tr("Clear Log"), this, &Me::clearLog, ":/toolbar/clear_log");
    _actnShowHelp = A_(tr("Help"), this, &Me::showHelp, ":/toolbar/help");
    _actnParamAdd = A_(tr("Add Parameter..."), this, &Me::createParameter, ":/toolbar/param_add");
    _actnParamDelete = A_(tr("Delete..."), this, &Me::deleteParameter, ":/toolbar/param_delete");
    _actnParamDescr = A_(tr("Annotate..."), this, &Me::annotateParameter, ":/toolbar/param_annotate");
    _actnParamMoveUp = A_(tr("Move Up"), this, &Me::moveParameterUp, ":/toolbar/move_up");
    _actnParamMoveDown = A_(tr("Move Down"), this, &Me::moveParameterDown, ":/toolbar/move_down");
    #undef A_
}

void ElemFormulaEditor::createToolbar()
{
    _toolbar = Z::Gui::makeToolBar({
        _actnApplyChanges, _actnResetChanges, 0,
        _actnParamAdd, 0,
        Ori::Gui::textToolButton(_actnCheckCode), _actnClearLog, 0,
        _actnShowHelp,
    });
}

void ElemFormulaEditor::populateWindowMenu(QMenu* menu)
{
    menu->addAction(_actnApplyChanges);
    menu->addAction(_actnResetChanges);
    menu->addSeparator();
    menu->addAction(_actnCheckCode);
    menu->addAction(_actnClearLog);
    menu->addSeparator();
    menu->addAction(_actnParamAdd);
}

void ElemFormulaEditor::populate()
{
    _lockEvents = true;
    qDebug() << "Populate values";
    _paramsEditor->populateValues();
    _codeEditor->setPlainText(_workingCopy->formula());
    updateParamsEditorVisibility();
    _lockEvents = false;
}

void ElemFormulaEditor::collect()
{
    if (!_isChanged) return;
    applyWorkingValues();
    _sourceElem->assign(_workingCopy);
    _isChanged = false;
    emit onApply();
}

void ElemFormulaEditor::reset()
{
    if (!_isChanged) return;
    if (!Ori::Dlg::yes(tr("Element <b>%1</b>: all changes "
        "made in this editor window will be lost. Continue?")
        .arg(_sourceElem->displayLabel()))) return;
    qDebug() << "Remove editors";
    _paramsEditor->removeEditors();
    _workingCopy->assign(_sourceElem);
    qDebug() << "Populate editors";
    _paramsEditor->populateEditors(_workingCopy->params());
    populate();
    _isChanged = false;
    emit onModify();
}

void ElemFormulaEditor::applyWorkingValues()
{
    _workingCopy->setFormula(_codeEditor->toPlainText());
    ElementMatrixLocker matrixLocker(_workingCopy, "ElemFormulaEditor::applyWorkingValues");
    _paramsEditor->applyValues();
}

void ElemFormulaEditor::paramsChanged()
{
    if (_lockEvents) return;
    _isChanged = true;
    emit onModify();
}

void ElemFormulaEditor::codeModified()
{
    if (_lockEvents) return;
    _isChanged = true;
    emit onModify();
}

void ElemFormulaEditor::resetModifyFlag()
{
    _codeEditor->document()->setModified(false);
}

void ElemFormulaEditor::checkFormula()
{
    applyWorkingValues();

    if (_workingCopy->ok())
        _logView->setHtml(Z::Format::matrices(_workingCopy->Mt(), _workingCopy->Ms()));
    else
        _logView->setHtml(QString("<p style='color:red'>%1").arg(_workingCopy->error()));
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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addRow(new QLabel(tr("Name")), aliasEditor);
    layout->addRow(new QLabel(tr("Dim")), dimEditor);

    auto verifyFunc = [&](){
        auto alias = aliasEditor->text().trimmed();
        if (alias.isEmpty())
            return tr("Parameter name can't be empty");
        if (_workingCopy->params().byAlias(alias))
            return tr("Parameter <b>%1</b> already exists").arg(alias);
        if (!Z::FormulaUtils::isValidVariableName(alias))
            return tr("Parameter name <b>%1</b> is invalid").arg(alias);
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
        _workingCopy->addParam(param);
        _paramsEditor->addEditor(param);
        _paramsEditor->populateEditor(param);
        _paramsEditor->focus(param);
        if (_stubNoParams->isVisible())
        {
            _stubNoParams->setVisible(false);
            _paramsEditor->setVisible(true);
        }
        paramsChanged();
    }
}

void ElemFormulaEditor::deleteParameter()
{
    auto param = _actnParamDelete->data().value<ParamEditor*>()->parameter();
    if (Ori::Dlg::yes(tr("Delete parameter <b>%1</b>?").arg(param->alias())))
    {
        // It's an action handler called from the menu owned by the editor we're trying to remove,
        // so it needes to be finished for action handler workflow before the editor can be freed.
        QTimer::singleShot(0, [this, param](){
            _paramsEditor->removeEditor(param);
            _workingCopy->removeParam(param);
            updateParamsEditorVisibility();
            paramsChanged();
        });
    }
}

void ElemFormulaEditor::updateParamsEditorVisibility()
{
    if (!_workingCopy->hasParams())
    {
        _stubNoParams->setVisible(true);
        _paramsEditor->setVisible(false);
    }
    else
    {
        _stubNoParams->setVisible(false);
        _paramsEditor->setVisible(true);
        _paramsEditor->focus(_workingCopy->params().first());
    }
}

void ElemFormulaEditor::annotateParameter()
{
    auto param = _actnParamDelete->data().value<ParamEditor*>()->parameter();
    QString newDescr = Ori::Dlg::inputText(tr("Parameter description:"), param->description());
    if (newDescr != param->description())
    {
        param->setDescription(newDescr);
        _paramsEditor->focus(param);
        paramsChanged();
    }
}

void ElemFormulaEditor::moveParameterUp()
{
    if (_workingCopy->params().count() < 2) return;
    auto param = _actnParamMoveUp->data().value<ParamEditor*>()->parameter();
    _paramsEditor->moveEditorUp(param);
    _workingCopy->moveParamUp(param);
    paramsChanged();
}

void ElemFormulaEditor::moveParameterDown()
{
    if (_workingCopy->params().count() < 2) return;
    auto param = _actnParamMoveDown->data().value<ParamEditor*>()->parameter();
    _paramsEditor->moveEditorDown(param);
    _workingCopy->moveParamDown(param);
    paramsChanged();
}
