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

} // namespace VariableEditor
