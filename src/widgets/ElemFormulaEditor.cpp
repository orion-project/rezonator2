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
#include <QTimer>
#include <QToolButton>
#include <QPlainTextEdit>

using namespace Ori::Layouts;

template <class TPayload>
class ObjectDeleter : public QObject
{
public:
    ObjectDeleter(TPayload* payload, QObject* parent) : QObject(parent), _payload(payload) {}
    ~ObjectDeleter() override { delete _payload; }
private:
    TPayload* _payload;
};


ElemFormulaEditor::ElemFormulaEditor(ElemFormula* sourceElem, bool fullToolbar)
    : ElemFormulaEditor(sourceElem, nullptr, fullToolbar)
{
}

ElemFormulaEditor::ElemFormulaEditor(ElemFormula* sourceElem, ElemFormula *workingCopy, bool fullToolbar)
    : QWidget(), _sourceElem(sourceElem), _workingCopy(workingCopy)
{
    if (!_workingCopy)
    {
        _workingCopy = new ElemFormula;
        initWorkingCopy();
    }

    createActions();
    createToolbar(fullToolbar);

    ParamsEditor::Options opts(&_workingCopy->params());
    opts.menuButtonActions = {_actnParamDescr, nullptr, _actnParamMoveUp, _actnParamMoveDown, nullptr, _actnParamDelete};
    _paramsEditor = new ParamsEditor(opts);
    _paramsEditor->setVisible(!_workingCopy->params().isEmpty());
    connect(_paramsEditor, &ParamsEditor::paramChanged, this, &ElemFormulaEditor::editorChanged);

    _stubNoParams = LayoutV({
        LayoutH({ Stretch(), new QLabel(tr("Element nas no parameters")), Stretch() }),
        Stretch(),
    }).makeWidget();
    _stubNoParams->setVisible(_workingCopy->params().isEmpty());

    _flagHasMatricesTS = new QCheckBox(tr("Different matrices for T and S"));
    connect(_flagHasMatricesTS, &QCheckBox::stateChanged, this, &ElemFormulaEditor::editorChanged);

    auto paramsPanel = LayoutV({
        Z::Gui::headerlabel(tr(" Options")),
        LayoutV({ _flagHasMatricesTS }).setMargin(6),
        Space(6),
        Z::Gui::headerlabel(tr(" Parameters")),
        _paramsEditor,
        _stubNoParams,
    }).setMargin(0).makeWidget();

    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setFont(Z::Gui::CodeEditorFont().get());
    connect(_codeEditor, &QTextEdit::textChanged, this, &ElemFormulaEditor::editorChanged);

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

    // Element should be deleted only after all children, so put at the end
    new ObjectDeleter<Element>(_workingCopy, this);
}

ElemFormulaEditor::~ElemFormulaEditor()
{
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
    _actnParamMoveUp = A_(tr("Move Up"), this, SLOT(moveParameterUp()), ":/toolbar/move_up");
    _actnParamMoveDown = A_(tr("Move Down"), this, SLOT(moveParameterDown()), ":/toolbar/move_down");

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

void ElemFormulaEditor::initWorkingCopy()
{
    _workingCopy->removeParams();
    for (const auto p : _sourceElem->params())
    {
        auto paramCopy = new Z::Parameter(p->dim(),
                                          p->alias(),
                                          p->label(),
                                          p->name(),
                                          p->description(),
                                          p->category(),
                                          p->visible());
        paramCopy->setValue(p->value());
        _workingCopy->addParam(paramCopy);
    }
    _workingCopy->setFormula(_sourceElem->formula());
    _workingCopy->setHasMatricesTS(_sourceElem->hasMatricesTS());
}

void ElemFormulaEditor::populateValues()
{
    _lockEvents = true;
    _paramsEditor->populateValues();
    _codeEditor->setPlainText(_workingCopy->formula());
    _flagHasMatricesTS->setChecked(_workingCopy->hasMatricesTS());
    _lockEvents = false;
}

void ElemFormulaEditor::applyValues()
{
    _paramsEditor->applyValues();
    _workingCopy->setFormula(_codeEditor->toPlainText());
    _workingCopy->setHasMatricesTS(_flagHasMatricesTS->isChecked());
}

void ElemFormulaEditor::editorChanged()
{
    if (_lockEvents) return;
    if (_isChanged && !_firstChange) return;
    _isChanged = true;
    _firstChange = false;
    emit onChanged();
}

void ElemFormulaEditor::saveChanges()
{
    // TODO
    if (not _isChanged) return;
    _logView->append("Save changes");
    _isChanged = false;
    emit onChanged();
}

void ElemFormulaEditor::resetChanges()
{
    if (not _isChanged) return;
    if (not Ori::Dlg::yes(tr("Element <b>%1</b>: all changes "
        "made in this editor window will be lost. Continue?")
        .arg(_sourceElem->displayLabel()))) return;
    _paramsEditor->removeEditors();
    initWorkingCopy();
    _paramsEditor->populateEditors();
    populateValues();
    updateParamsEditorVisibility();
    _isChanged = false;
    emit onChanged();
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
        editorChanged();
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
            editorChanged();
        });
    }
}

void ElemFormulaEditor::updateParamsEditorVisibility()
{
    if (not _workingCopy->hasParams())
    {
        _stubNoParams->setVisible(true);
        _paramsEditor->setVisible(false);
    }
    else
        _paramsEditor->focus(_workingCopy->params().first());
}

void ElemFormulaEditor::annotateParameter()
{
    auto param = _actnParamDelete->data().value<ParamEditor*>()->parameter();
    QString newDescr = Ori::Dlg::inputText(tr("Parameter description:"), param->description());
    if (newDescr != param->description())
    {
        param->setDescription(newDescr);
        _paramsEditor->focus(param);
        editorChanged();
    }
}

void ElemFormulaEditor::moveParameterUp()
{
    if (_workingCopy->params().count() < 2) return;
    auto param = _actnParamMoveUp->data().value<ParamEditor*>()->parameter();
    _paramsEditor->moveEditorUp(param);
    _workingCopy->moveParamUp(param);
    editorChanged();
}

void ElemFormulaEditor::moveParameterDown()
{
    if (_workingCopy->params().count() < 2) return;
    auto param = _actnParamMoveDown->data().value<ParamEditor*>()->parameter();
    _paramsEditor->moveEditorDown(param);
    for (auto p : _workingCopy->params())
        qDebug() << "before" << p->alias();
    _workingCopy->moveParamDown(param);
    for (auto p : _workingCopy->params())
        qDebug() << "after" << p->alias();
    editorChanged();
}
