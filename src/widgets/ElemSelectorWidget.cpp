#include "ElemSelectorWidget.h"

#include "ValueEditor.h"
#include "../app/Appearance.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QLabel>
#include <QListWidget>
#include <QToolButton>

//------------------------------------------------------------------------------
//                              ElemSelectorWidget
//------------------------------------------------------------------------------

ElemSelectorWidget::ElemSelectorWidget(Schema* schema, const Options &opts) : QComboBox()
{
    setFont(Z::Gui::ValueFont().get());

    Elements elems(schema->elements());

    if (opts.includeCustomParams && !schema->globalParamsAsElem()->params().isEmpty())
        elems.append(const_cast<Element*>(schema->globalParamsAsElem()));

    for (auto elem : std::as_const(elems))
        if (!opts.filter || opts.filter->check(elem)) {
            _elements.append(elem);
            addItem(elem->displayLabelTitle());
        }
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

ParamSelectorWidget::ParamSelectorWidget(Z::ParameterFilterPtr filter) : QComboBox(), _filter(filter)
{
    setFont(Z::Gui::ValueFont().get());
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

    foreach (auto param, _parameters)
    {
        // Don't use param->label() because combo box items do not support HTML formatting (e.g. n<sub>0</sub>)
        auto alias = param->alias();
        auto name = param->name();
        auto display = name.isEmpty() || name == alias
            ? QStringLiteral("%1 = %2").arg(alias, param->value().displayStr())
            : QStringLiteral("%1 (%2) = %3").arg(alias, name, param->value().displayStr());
        addItem(display);
    }
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

ElemAndParamSelector::ElemAndParamSelector(Schema *schema, const Options &opts) : QGridLayout()
{
    _elemSelector = new ElemSelectorWidget(schema, {
        .filter = opts.elemFilter,
        .includeCustomParams = opts.includeCustomParams,
    });

    _paramSelector = new ParamSelectorWidget(opts.paramFilter);
    _paramSelector->populate(_elemSelector->selectedElement());

    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentElemChanged(int)));
    connect(_paramSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentParamChanged(int)));

    setContentsMargins(0, 0, 0, 0);
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

MultiElementSelectorWidget::MultiElementSelectorWidget(Schema* schema, ElementFilterPtr filter) : QWidget()
{
    _elemsSelector = new QListWidget;
    _elemsSelector->addAction(Ori::Gui::V0::action("", this, SLOT(selectAllElements()), "", Qt::CTRL|Qt::Key_A));
    _elemsSelector->addAction(Ori::Gui::V0::action("", this, SLOT(deselectAllElements()), "", Qt::CTRL|Qt::Key_D));
    _elemsSelector->addAction(Ori::Gui::V0::action("", this, SLOT(invertElementsSelection()), "", Qt::CTRL|Qt::Key_I));
    connect(_elemsSelector, &QListWidget::currentItemChanged, this, &MultiElementSelectorWidget::currentItemChanged);
    connect(_elemsSelector, &QListWidget::itemDoubleClicked, this, &MultiElementSelectorWidget::invertCheckState);
    connect(_elemsSelector, &QListWidget::itemClicked, this, [&](QListWidgetItem *item){
        if (!item->isSelected()) _elemsSelector->setCurrentItem(item); });

    Ori::Layouts::LayoutH({
        _elemsSelector,
        Ori::Layouts::LayoutV({
            Ori::Gui::iconToolButton(tr("Select All"), ":/toolbar/check_all", this, SLOT(selectAllElements())),
            Ori::Gui::iconToolButton(tr("Select None"), ":/toolbar/check_none", this, SLOT(deselectAllElements())),
            Ori::Gui::iconToolButton(tr("Invert Selection"), ":/toolbar/check_invert", this, SLOT(invertElementsSelection())),
            Ori::Layouts::Stretch()
        })
    })
    .setMargin(0)
    .useFor(this);

    populate(schema, filter);
}

void MultiElementSelectorWidget::populate(Schema* schema, ElementFilterPtr filter)
{
    for (auto elem : schema->elements())
    {
        if (filter && !filter->check(elem)) continue;

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

void MultiElementSelectorWidget::selectElement(Element* elem)
{
    if (_itemsMap.contains(elem))
        _itemsMap[elem]->setCheckState(Qt::Checked);
}

Elements MultiElementSelectorWidget::selectedElements() const
{
    Elements res;
    for (int i = 0; i < _elemsSelector->count(); i++)
        if (_elemsSelector->item(i)->checkState() == Qt::Checked)
            res << element(_elemsSelector->item(i));
    return res;
}

Element* MultiElementSelectorWidget::currentElement() const
{
    return element(_elemsSelector->currentItem());
}

void MultiElementSelectorWidget::setCurrentRow(int index)
{
    _elemsSelector->setCurrentRow(index);
}

//------------------------------------------------------------------------------
//                         ElementOffsetSelectorWidget
//------------------------------------------------------------------------------

ElemOffsetSelectorWidget::ElemOffsetSelectorWidget(Schema* schema, ElementFilterPtr filter)
{
    _elemSelector = new ElemSelectorWidget(schema, { .filter = filter });
    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(currentElemChanged(int)));

    _lengthTitle = new QLabel(tr("Length"));

    _lengthLabel = new QLabel;
    _lengthLabel->setFont(Z::Gui::ValueFont().get());

    _offsetTitle = new QLabel(tr("Offset"));
    _offsetEditor = new ValueEditor;
    currentElemChanged(-1);

    setColumnStretch(1, 1);
    addWidget(new QLabel(tr("Element")), 0, 0); addWidget(_elemSelector, 0, 1);
    addWidget(_lengthTitle, 1, 0); addWidget(_lengthLabel, 1, 1);
    addWidget(_offsetTitle, 2, 0); addWidget(_offsetEditor, 2, 1);
}

void ElemOffsetSelectorWidget::currentElemChanged(int)
{
    auto range = Z::Utils::asRange(selectedElement());
    _lengthTitle->setEnabled(range);
    _lengthLabel->setEnabled(range);
    _offsetTitle->setEnabled(range);
    _offsetEditor->setEnabled(range);
    if (range)
    {
        _lengthLabel->setText(range->axisLen().displayStr());
        _offsetEditor->setValue(Z::Value(0, range->paramLength()->value().unit()));
    }
    else
    {
        _lengthLabel->setText(QStringLiteral("N/A"));
        _offsetEditor->setValue(0);
    }
}

Z::Value ElemOffsetSelectorWidget::offset() const
{
    return _offsetEditor->value();
}

void ElemOffsetSelectorWidget::setOffset(const Z::Value& offset)
{
    _offsetEditor->setValue(offset);
}

WidgetResult ElemOffsetSelectorWidget::verify()
{
    auto elem = selectedElement();
    if (!elem)
        WidgetResult::fail(_elemSelector, tr("An element must be selected."));

    auto range = Z::Utils::asRange(elem);
    if (!range) return WidgetResult::ok();

    auto length = range->paramLength()->value();
    auto offset = this->offset();

    if (offset < 0)
        return WidgetResult::fail(_offsetEditor,
            tr("The offset value cannot be less than zero."));

    if (offset > length)
        return WidgetResult::fail(_offsetEditor,
            tr("The offset cannot be greater than element length."));

    return WidgetResult::ok();
}
