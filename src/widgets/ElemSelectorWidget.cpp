#include "ElemSelectorWidget.h"

#include "Appearance.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QLabel>
#include <QListWidget>
#include <QToolButton>

//------------------------------------------------------------------------------
//                              ElemSelectorWidget
//------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
//                           MultiElementSelectorWidget
//------------------------------------------------------------------------------

MultiElementSelectorWidget::MultiElementSelectorWidget(Schema* schema, ElementFilter *filter) : QWidget()
{
    _elemsSelector = new QListWidget;
    _elemsSelector->addAction(Ori::Gui::action("", this, SLOT(selectAllElements()), "", Qt::CTRL+Qt::Key_A));
    _elemsSelector->addAction(Ori::Gui::action("", this, SLOT(deselectAllElements()), "", Qt::CTRL+Qt::Key_D));
    _elemsSelector->addAction(Ori::Gui::action("", this, SLOT(invertElementsSelection()), "", Qt::CTRL+Qt::Key_I));
    connect(_elemsSelector, &QListWidget::currentItemChanged, this, &MultiElementSelectorWidget::currentItemChanged);
    connect(_elemsSelector, &QListWidget::itemDoubleClicked, this, &MultiElementSelectorWidget::invertCheckState);
    connect(_elemsSelector, &QListWidget::itemClicked, [&](QListWidgetItem *item){
        if (!item->isSelected()) _elemsSelector->setCurrentItem(item); });

    Ori::Layouts::LayoutH({
        _elemsSelector,
        Ori::Layouts::LayoutV({
            Ori::Gui::iconToolButton(tr("Select All"), ":/toolbar16/check_all", this, SLOT(selectAllElements())),
            Ori::Gui::iconToolButton(tr("Select None"), ":/toolbar16/check_none", this, SLOT(deselectAllElements())),
            Ori::Gui::iconToolButton(tr("Invert Selection"), ":/toolbar16/check_invert", this, SLOT(invertElementsSelection())),
            Ori::Layouts::Stretch()
        })
    })
    .setMargin(0)
    .useFor(this);

    populate(schema, filter);
}

void MultiElementSelectorWidget::populate(Schema* schema, ElementFilter *filter)
{
    for (auto elem : schema->elements())
    {
        if (filter and !filter->check(elem)) continue;

        auto item = new QListWidgetItem(elem->displayLabelTitle());
        item->setIcon(QIcon(Z::Utils::elemIconPath(elem)));
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, QVariant::fromValue<void*>(elem));
        _itemsMap[elem] = item;
        _elemsSelector->addItem(item);
    }
}

Element* MultiElementSelectorWidget::element(QListWidgetItem *item) const
{
    return item ? reinterpret_cast<Element*>(item->data(Qt::UserRole).value<void*>()) : nullptr;
}

void MultiElementSelectorWidget::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    emit currentElementChanged(element(current), element(previous));
}

void MultiElementSelectorWidget::invertCheckState(QListWidgetItem *item)
{
    item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}

void MultiElementSelectorWidget::selectAllElements()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        _elemsSelector->item(i)->setCheckState(Qt::Checked);
}

void MultiElementSelectorWidget::deselectAllElements()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        _elemsSelector->item(i)->setCheckState(Qt::Unchecked);
}

void MultiElementSelectorWidget::invertElementsSelection()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        invertCheckState(_elemsSelector->item(i));
}

Elements MultiElementSelectorWidget::allElements() const
{
   return _itemsMap.keys();
}

void MultiElementSelectorWidget::select(Element* elem)
{
    if (_itemsMap.contains(elem))
        _itemsMap[elem]->setCheckState(Qt::Checked);
}

Elements MultiElementSelectorWidget::selected() const
{
    Elements res;
    for (int i = 0; i < _elemsSelector->count(); i++)
        if (_elemsSelector->item(i)->checkState() == Qt::Checked)
            res << element(_elemsSelector->item(i));
    return res;
}

Element* MultiElementSelectorWidget::current() const
{
    return element(_elemsSelector->currentItem());
}

void MultiElementSelectorWidget::setCurrentRow(int index)
{
    _elemsSelector->setCurrentRow(index);
}
