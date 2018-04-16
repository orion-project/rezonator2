#include "FormulaEditor.h"

#include "Appearance.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriLayouts.h"

#include <QLabel>
#include <QTextEdit>
#include <QTimer>

FormulaEditor::FormulaEditor(Z::Formula *formula, QWidget *parent) : QTabWidget(parent), _formula(formula)
{
    _codeEditor = new QTextEdit;
    _codeEditor->setAcceptRichText(false);
    _codeEditor->setPlainText(_formula->code());
    Ori::Gui::setFontMonospace(_codeEditor);
    connect(_codeEditor, &QTextEdit::textChanged, this, &FormulaEditor::codeChanged);

    _statusLabel = new QLabel;
    _statusLabel->setWordWrap(true);

    auto codeTab = Ori::Layouts::LayoutV({ _codeEditor, _statusLabel }).makeWidget();
    auto paramsTab = new QWidget();

    addTab(codeTab, tr("Code"));
    addTab(paramsTab, tr("Params"));

    _recalcTimer = new QTimer(this);
    _recalcTimer->setInterval(250);
    _recalcTimer->setSingleShot(true);
    connect(_recalcTimer, &QTimer::timeout, this, &FormulaEditor::calculate);

    _formula->calculate();
    showStatus();
}

void FormulaEditor::setFocus()
{
    setCurrentIndex(0);
    _codeEditor->setFocus();
}

void FormulaEditor::codeChanged()
{
    _recalcTimer->start();
}

void FormulaEditor::showStatus()
{
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

void FormulaEditor::calculate()
{
    _formula->setCode(_codeEditor->toPlainText());
    _formula->calculate();
    showStatus();
}
