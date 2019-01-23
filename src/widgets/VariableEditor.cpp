#include "VariableEditor.h"

#include "ElemSelectorWidget.h"
#include "VariableRangeEditor.h"
#include "../core/Variable.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QApplication>
#include <QDebug>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>

using namespace Ori::Gui;
using namespace Ori::Layouts;

namespace VariableEditor {

//------------------------------------------------------------------------------
//                                ElementEd

ElementEd::ElementEd(Schema *schema) : QVBoxLayout()
{
    _elemFilter.reset(ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>());
    _elemSelector = new ElemAndParamSelector(schema, _elemFilter.get(), Z::Utils::defaultParamFilter());

    _rangeEditor = new VariableRangeEditor::GeneralRangeEd;

    connect(_elemSelector, SIGNAL(selectionChanged()), this, SLOT(guessRange()));

    addLayout(_elemSelector);
    addSpacing(8);
    addWidget(group(tr("Variation"), _rangeEditor));
}

void ElementEd::guessRange()
{
    auto param = _elemSelector->selectedParameter();
    if (!param) return;

    // TODO restore or guess range limits and step
    Z::VariableRange range;
    range.start = param->value();
    range.stop = param->value();
    range.step = param->value() * 0;
    _rangeEditor->setRange(range);
}

void ElementEd::populate(Z::Variable *var)
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

void ElementEd::collect(Z::Variable *var)
{
    var->element = _elemSelector->selectedElement();
    var->parameter = _elemSelector->selectedParameter();
    var->range = _rangeEditor->range();
}

WidgetResult ElementEd::verify()
{
    auto res = _elemSelector->verify();
    return res ? _rangeEditor->verify() : res;
}

//------------------------------------------------------------------------------
//                              ElementRangeEd

ElementRangeEd::ElementRangeEd(Schema *schema) : QVBoxLayout()
{
    _elemFilter.reset(ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());
    _elemSelector = new ElemSelectorWidget(schema, _elemFilter.get());
    _rangeEditor = new VariableRangeEditor::PointsRangeEd;

    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(guessRange()));

    auto layoutElement = new QHBoxLayout;
    layoutElement->addWidget(new QLabel(tr("Element")));
    layoutElement->addSpacing(8);
    layoutElement->addWidget(_elemSelector);
    layoutElement->setStretch(2, 1);

    addLayout(layoutElement);
    addSpacing(8);
    addWidget(group(tr("Plot accuracy"), _rangeEditor));
}

void ElementRangeEd::guessRange()
{
    auto elem = _elemSelector->selectedElement();
    if (!elem) return;

    auto elemRange = Z::Utils::asRange(elem);
    if (!elemRange) return;

    // TODO restore or guess step
    Z::VariableRange range;
    range.stop = Z::Utils::getRangeStop(elemRange);
    range.start = range.stop * 0.0;
    range.step = range.stop / 100.0;
    _rangeEditor->setRange(range);
}

void ElementRangeEd::populate(Z::Variable *var)
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

void ElementRangeEd::collect(Z::Variable *var)
{
    var->element = _elemSelector->selectedElement();
    var->parameter = Z::Utils::asRange(var->element)->paramLength();
    var->range = _rangeEditor->range();
}

WidgetResult ElementRangeEd::verify()
{
    auto res = _elemSelector->verify();
    return res ? _rangeEditor->verify() : res;
}

//------------------------------------------------------------------------------
//                              MultiElementRangeEd

MultiElementRangeEd::MultiElementRangeEd(Schema *schema) : QWidget()
{
    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());

    _elemsSelector = new MultiElementSelectorWidget(schema, elemFilter.get());
    connect(_elemsSelector, &MultiElementSelectorWidget::currentElementChanged,
            this, &MultiElementRangeEd::currentElementChanged);

    // Make default ranges for all elements
    for (auto elem : _elemsSelector->allElements())
    {
        Z::VariableRange range;
        range.stop = Z::Utils::getRangeStop(Z::Utils::asRange(elem));
        range.start = Z::Value(0, range.stop.unit());
        range.step = range.stop / 100.0;
        range.useStep = false;
        range.points = 100;
        _elemRanges[elem] = range;
    }

    _sameSettings = new QCheckBox(tr("Use these settings for all elements"));

    _rangeEditor = new VariableRangeEditor::PointsRangeEd;
    _rangeEditor->addWidget(_sameSettings, _rangeEditor->rowCount()+1, 0, 1, _rangeEditor->columnCount());

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(_elemsSelector);
    layout->addSpacing(4);
    layout->addWidget(group(tr("Plot accuracy"), _rangeEditor));
    setLayout(layout);
}

void MultiElementRangeEd::populateVars(const QVector<Z::Variable>& vars)
{
    for (const Z::Variable& var : vars)
        if (_elemRanges.contains(var.element))
        {
            _elemsSelector->select(var.element);
            _elemRanges[var.element] = var.range;
        }
    _elemsSelector->setCurrentRow(0);
}

QVector<Z::Variable> MultiElementRangeEd::collectVars()
{
    saveEditedRange(_elemsSelector->current());

    QVector<Z::Variable> vars;
    for (auto elem : _elemsSelector->selected())
    {
        Z::Variable var;
        var.element = elem;
        var.parameter = Z::Utils::asRange(elem)->paramLength();
        var.range = _elemRanges[elem];
        vars.append(var);
    }
    return vars;
}

WidgetResult MultiElementRangeEd::verify()
{
    if (_elemsSelector->selectedCount() == 0)
        return WidgetResult::fail(_elemsSelector,
            tr("No elements are chosen.\nYou should mark at least one element."));

    return WidgetResult::ok(); // TODO
}

void MultiElementRangeEd::currentElementChanged(Element *current, Element *previous)
{
    saveEditedRange(previous);
    showRangeInEditor(current);
}

void MultiElementRangeEd::showRangeInEditor(Element *elem)
{
    if (_elemRanges.contains(elem))
        _rangeEditor->setRange(_elemRanges[elem]);
}

void MultiElementRangeEd::saveEditedRange(Element *elem)
{
    auto newRange = _rangeEditor->range();
    if (_sameSettings->isChecked())
        for (auto elem : _elemRanges.keys())
            _elemRanges[elem].assignPoints(newRange);
    else if (_elemRanges.contains(elem))
        _elemRanges[elem].assignPoints(newRange);
}

} // namespace VariableEditor
