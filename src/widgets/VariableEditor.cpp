#include "VariableEditor.h"
#include "ElemSelectorWidget.h"
#include "VariableRangeWidget.h"
#include "../core/Variable.h"

#include <QApplication>
#include <QGroupBox>
#include <QLabel>

void populateVariableEditorLayout(QVBoxLayout* layout, QLayout* elemSelector, QLayout* rangeEditor, const QString& rangeTitle)
{
    auto groupRange = new QGroupBox(rangeTitle);
    groupRange->setLayout(rangeEditor);

    layout->addLayout(elemSelector);
    layout->addSpacing(8);
    layout->addWidget(groupRange);
}

//------------------------------------------------------------------------------
//                                VariableEditor

VariableEditor::VariableEditor(Schema *schema) : QVBoxLayout()
{
    _elemFilter.reset(ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>());
    _elemSelector = new ElemAndParamSelector(schema, _elemFilter.get(), Z::Utils::defaultParamFilter());

    _rangeEditor = new VariableRangeWidget;

    connect(_elemSelector, SIGNAL(selectionChanged()), this, SLOT(guessRange()));

    populateVariableEditorLayout(this, _elemSelector, _rangeEditor, tr("Variation"));
}

void VariableEditor::guessRange()
{
    auto param = _elemSelector->selectedParameter();
    if (!param) return;

    // TODO restore or guess range limits and step
    Z::VariableRange range;
    range.start = param->value();
    range.stop = param->value();
    range.step = Z::Value(0, param->value().unit());
    _rangeEditor->setRange(range);
}

void VariableEditor::populate(Z::Variable *var)
{
    if (var->element) // edit variable
    {
        _elemSelector->setSelectedElement(var->element);
        _elemSelector->setSelectedParameter(var->parameter);
        _rangeEditor->setRange(var->range);
    }
    else // 'create' variable
    {
        // TODO guess or restore from settings
        guessRange();
    }
}

void VariableEditor::collect(Z::Variable *var)
{
    var->element = _elemSelector->selectedElement();
    var->parameter = _elemSelector->selectedParameter();
    var->range = _rangeEditor->range();
}

WidgetResult VariableEditor::verify()
{
    auto res = _elemSelector->verify();
    return res ? _rangeEditor->verify() : res;
}

//------------------------------------------------------------------------------
//                          VariableEditor_ElementRange

VariableEditor_ElementRange::VariableEditor_ElementRange(Schema *schema) : QVBoxLayout()
{
    _elemFilter.reset(ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());
    _elemSelector = new ElemSelectorWidget(schema, _elemFilter.get());
    _rangeEditor = new VariableRangeWidget_ElementRange;

    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(guessRange()));

    auto layoutElement = new QHBoxLayout;
    layoutElement->addWidget(new QLabel(tr("Element")));
    layoutElement->addSpacing(8);
    layoutElement->addWidget(_elemSelector);
    layoutElement->setStretch(2, 1);

    populateVariableEditorLayout(this, layoutElement, _rangeEditor, tr("Plot accuracy"));
}

void VariableEditor_ElementRange::guessRange()
{
    auto elem = _elemSelector->selectedElement();
    if (!elem) return;

    auto elemRange = Z::Utils::asRange(elem);
    if (!elemRange) return;

    auto param = elemRange->paramLength();

    // TODO restore or guess step
    Z::VariableRange range;
    range.start = Z::Value(0, param->value().unit());
    range.stop = param->value();
    range.step = Z::Value(0, param->value().unit());
    _rangeEditor->setRange(range);
}

void VariableEditor_ElementRange::populate(Z::Variable *var)
{
    if (var->element) // edit variable
    {
        _elemSelector->setSelectedElement(var->element);
        _rangeEditor->setRange(var->range);
    }
    else // 'create' variable
    {
        // TODO guess or restore from settings
        guessRange();
    }
}

void VariableEditor_ElementRange::collect(Z::Variable *var)
{
    var->element = _elemSelector->selectedElement();
    var->parameter = Z::Utils::asRange(var->element)->paramLength();
    var->range = _rangeEditor->range();
    qDebug() << "range.start" << var->range.start.str();
}

WidgetResult VariableEditor_ElementRange::verify()
{
    auto res = _elemSelector->verify();
    return res ? _rangeEditor->verify() : res;
}
