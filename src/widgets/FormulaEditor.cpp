#include "FormulaEditor.h"

#include "../app/Appearance.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>

using namespace Ori::Layouts;

#define RECALCULATE_AFTER_TYPE_INTERVAL_MS 250

FormulaEditor::FormulaEditor(Options opts, QWidget *parent)
    : QWidget(parent), _formula(opts.formula), _globalParams(opts.globalParams), _formulas(opts.formulas)
{
    _targetParam = opts.targetParam ? opts.targetParam : _formula->target();

    _recalcTimer = new QTimer(this);
    _recalcTimer->setSingleShot(true);
    _recalcTimer->setInterval(RECALCULATE_AFTER_TYPE_INTERVAL_MS);
    connect(_recalcTimer, &QTimer::timeout, this, &FormulaEditor::calculate);

    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setPlainText(_formula->code());
    _codeEditor->setFont(Z::Gui::CodeEditorFont().get());
    connect(_codeEditor, &QTextEdit::textChanged, _recalcTimer, QOverload<>::of(&QTimer::start));

    _statusLabel = new QLabel;
    _statusLabel->setWordWrap(true);

    LayoutV({ _codeEditor, _statusLabel }).setMargin(0).useFor(this);
}

void FormulaEditor::setFocus()
{
    _codeEditor->setFocus();
}

void FormulaEditor::calculate()
{
    _formula->setCode(_codeEditor->toPlainText());
    _formula->findDeps(*_globalParams);
    _formula->calculate();
    if (_formula->ok())
    {
        _statusLabel->setText("OK");
        _statusLabel->setStyleSheet("QLabel{background:LightGreen;padding:3px}");
    }
    else
    {
        _statusLabel->setText(_formula->error());
        _statusLabel->setStyleSheet("QLabel{background:LightCoral;padding:3px}");
    }
}
