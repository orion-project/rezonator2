#include "Appearance.h"
#include "ElemSelectorWidget.h"

#include <QLabel>

//------------------------------------------------------------------------------
//                              ElemSelectorWidget

ElemSelectorWidget::ElemSelectorWidget(Schema* schema, ElementFilter *filter) : QComboBox(), _filter(filter)
{
    Z::Gui::setValueFont(this);

    for (auto elem : schema->elements())
        if (!filter || filter->check(elem))
            _elements.append(elem);

    for (auto elem : _elements)
        addItem(elem->displayLabelTitle());
}

Element* ElemSelectorWidget::selectedElement() const
{
    return currentIndex() > -1 ? _elements.at(currentIndex()): nullptr;
}

void ElemSelectorWidget::setSelectedElement(Element *elem)
{
    setCurrentIndex(_elements.indexOf(elem));
}

WidgetResult ElemSelectorWidget::verify()
{
    return currentIndex() < 0
        ? WidgetResult::fail(this, tr("An element must be selected."))
        : WidgetResult::ok();
}

//------------------------------------------------------------------------------
//                             ParamsSelectorWidget

ParamSelectorWidget::ParamSelectorWidget(Z::ParameterFilter* filter) : QComboBox(), _filter(filter)
{
    Z::Gui::setValueFont(this);
}

void ParamSelectorWidget::populate(Element *elem)
{
    clear();
    setCurrentIndex(-1);
    _parameters.clear();

    if (elem)
        for (auto param : elem->params())
            if (!_filter || _filter->check(param))
                _parameters.append(param);

    for (auto param : _parameters)
        addItem(param->name());
}

Z::Parameter* ParamSelectorWidget::selectedParameter() const
{
    return currentIndex() > -1 ? _parameters.at(currentIndex()) : nullptr;
}

void ParamSelectorWidget::setSelectedParameter(Z::Parameter* param)
{
    setCurrentIndex(_parameters.indexOf(param));
}

WidgetResult ParamSelectorWidget::verify()
{
    return currentIndex() < 0
        ? WidgetResult::fail(this, tr("A parameter must be selected."))
        : WidgetResult::ok();
}

//------------------------------------------------------------------------------
//                             ElemAndParamSelector

ElemAndParamSelector::ElemAndParamSelector(
    Schema *schema, ElementFilter *elemFilter, Z::ParameterFilter* paramFilter)
    : QGridLayout()
{
    _elemSelector = new ElemSelectorWidget(schema, elemFilter);
    _paramSelector = new ParamSelectorWidget(paramFilter);
    _paramSelector->populate(_elemSelector->selectedElement());

    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentElemChanged(int)));
    connect(_paramSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentParamChanged(int)));

    setMargin(0);
    setColumnStretch(1, 1);
    addWidget(new QLabel(tr("Element")), 0, 0);     addWidget(_elemSelector, 0, 1);
    addWidget(new QLabel(tr("Parameter")), 1, 0);   addWidget(_paramSelector, 1, 1);
}

void ElemAndParamSelector::currentElemChanged(int)
{
    _paramSelector->populate(selectedElement());
}

void ElemAndParamSelector::currentParamChanged(int)
{
    emit selectionChanged();
}

WidgetResult ElemAndParamSelector::verify()
{
    auto res = _elemSelector->verify();
    return res ? _paramSelector->verify() : res;
}
