#include "FormulaEditor.h"

#include "Appearance.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QTextEdit>
#include <QTimer>

#define RECALCULATE_AFTER_TYPE_INTERVAL_MS 250

FormulaEditor::FormulaEditor(Z::Formula *formula, QWidget *parent) : QTabWidget(parent), _formula(formula)
{
    _recalcTimer = new QTimer(this);
    _recalcTimer->setSingleShot(true);
    _recalcTimer->setInterval(RECALCULATE_AFTER_TYPE_INTERVAL_MS);
    connect(_recalcTimer, &QTimer::timeout, this, &FormulaEditor::calculate);

    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setPlainText(_formula->code());
    Z::Gui::setCodeEditorFont(_codeEditor);
    connect(_codeEditor, &QTextEdit::textChanged, _recalcTimer, QOverload<>::of(&QTimer::start));

    _statusLabel = new QLabel;
    _statusLabel->setWordWrap(true);

    auto codeTab = Ori::Layouts::LayoutV({ _codeEditor, _statusLabel }).makeWidget();
    auto paramsTab = new QWidget();

    addTab(codeTab, tr("Code"));
    addTab(paramsTab, tr("Params"));
}

void FormulaEditor::setFocus()
{
    setCurrentIndex(0);
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
