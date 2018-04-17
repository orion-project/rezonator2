#include "FormulaEditor.h"

#include "Appearance.h"
#include "ParamsListWidget.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>

using namespace Ori::Layouts;

#define RECALCULATE_AFTER_TYPE_INTERVAL_MS 250

FormulaEditor::FormulaEditor(Options opts, QWidget *parent)
    : QTabWidget(parent), _formula(opts.formula), _globalParams(opts.globalParams)
{
    _targetParam = opts.targetParam ? opts.targetParam : _formula->target();

    _recalcTimer = new QTimer(this);
    _recalcTimer->setSingleShot(true);
    _recalcTimer->setInterval(RECALCULATE_AFTER_TYPE_INTERVAL_MS);
    connect(_recalcTimer, &QTimer::timeout, this, &FormulaEditor::calculate);

    addTab(makeEditorTab(), tr("Code"));
    addTab(makeParamsTab(), tr("Params"));
}

QWidget* FormulaEditor::makeEditorTab()
{
    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setPlainText(_formula->code());
    Z::Gui::setCodeEditorFont(_codeEditor);
    connect(_codeEditor, &QTextEdit::textChanged, _recalcTimer, QOverload<>::of(&QTimer::start));

    _statusLabel = new QLabel;
    _statusLabel->setWordWrap(true);

    return LayoutV({ _codeEditor, _statusLabel }).makeWidget();
}

QWidget* FormulaEditor::makeParamsTab()
{
    auto buttonAdd = new QPushButton("+");
    buttonAdd->setFixedSize(32, 32);
    buttonAdd->setToolTip(tr("Add parameter"));
    connect(buttonAdd, &QPushButton::clicked, this, &FormulaEditor::addParam);

    auto buttonRemove = new QPushButton("-");
    buttonRemove->setFixedSize(32, 32);
    buttonRemove->setToolTip(tr("Remove parameter"));
    connect(buttonRemove, &QPushButton::clicked, this, &FormulaEditor::removeParam);

    _paramsList = new ParamsListWidget(&_formula->deps());

    return LayoutH({
        _paramsList,
        LayoutV({buttonAdd, buttonRemove, Stretch()})
    }).makeWidget();
}

void FormulaEditor::addParam()
{
    Z::Parameters availableParams;
    if (_globalParams)
        for (auto param : *_globalParams)
            if (param != _targetParam)
                if (!_formula->deps().byPointer(param))
                    availableParams.append(param);
    if (availableParams.isEmpty())
        return Ori::Dlg::info(tr("There are no parameters to add"));

    auto param = ParamsListWidget::selectParamDlg(&availableParams, tr("Add global parameter"));
    if (!param) return;

    // TODO check for circular dependencies
    _formula->addDep(param);
    _paramsList->addParamItem(param, true);
    calculate();
}

void FormulaEditor::removeParam()
{
    auto param = _paramsList->selectedParam();
    if (!param) return;

    _formula->removeDep(param);
    delete _paramsList->currentItem();

    calculate();
}

void FormulaEditor::setFocus()
{
    setCurrentIndex(0);
    // TODO it doesn't work, editor is not focused...
    _codeEditor->setFocus();
}

void FormulaEditor::calculate()
{
    _formula->setCode(_codeEditor->toPlainText());
    _formula->calculate();
    if (_formula->ok())
    {
        _statusLabel->setText("OK");
        _statusLabel->setStyleSheet("QLabel{background:LightGreen;padding:3px}");
    }
    else
    {
        _statusLabel->setText(_formula->status());
        _statusLabel->setStyleSheet("QLabel{background:LightCoral;padding:3px}");
    }
}
