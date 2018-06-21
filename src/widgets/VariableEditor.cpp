#include "VariableEditor.h"

#include "ElemSelectorWidget.h"
#include "VariableRangeEditor.h"
#include "../core/Variable.h"

#include <QApplication>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>

namespace VariableEditor {

QGroupBox* groupBox(const QString& title, QLayout* layout)
{
    auto group = new QGroupBox(title);
    group->setLayout(layout);
    return group;
}

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
    addWidget(groupBox(tr("Variation"), _rangeEditor));
}

void ElementEd::guessRange()
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
    addWidget(groupBox(tr("Plot accuracy"), _rangeEditor));
}

void ElementRangeEd::guessRange()
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
    qDebug() << "range.start" << var->range.start.str();
}

WidgetResult ElementRangeEd::verify()
{
    auto res = _elemSelector->verify();
    return res ? _rangeEditor->verify() : res;
}

//------------------------------------------------------------------------------
//                              MultiElementRangeEd

struct ElemItemData
{
    Element* element;
    QListWidgetItem* item;
    Z::VariableRange range;
};

MultiElementRangeEd::MultiElementRangeEd(Schema *schema) : QVBoxLayout()
{
    _elemsSelector = new QListWidget;
    _rangeEditor = new VariableRangeEditor::PointsRangeEd;

    connect(_elemsSelector, &QListWidget::currentItemChanged, this, &MultiElementRangeEd::currentItemChanged);
    connect(_elemsSelector, &QListWidget::itemDoubleClicked, this, &MultiElementRangeEd::itemDoubleClicked);

    // Collect available elements data
    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());
    for (auto elem : schema->elements())
        if (elemFilter->check(elem))
        {
            auto data = new ElemItemData;
            data->element = elem;
            data->range.stop = Z::Utils::asRange(elem)->paramLength()->value();
            data->range.start = data->range.stop * 0.0; // this preserves units
            data->range.step = data->range.stop / 100.0; // this preserves units
            data->range.useStep = false;
            data->range.points = 100;
            _itemsData.append(data);
        }

    // Fill elements selector
    for (int i = 0; i < _itemsData.size(); i++)
    {
        auto itemData = _itemsData.at(i);
        auto item = new QListWidgetItem(itemData->element->displayLabelTitle());
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, i);
        itemData->item = item;
        _elemsSelector->addItem(item);
    }

    addWidget(_elemsSelector);
    addSpacing(4);
    addWidget(groupBox(tr("Plot accuracy"), _rangeEditor));
}

MultiElementRangeEd::~MultiElementRangeEd()
{
    qDeleteAll(_itemsData);
}

void MultiElementRangeEd::populateVars(const QVector<Z::Variable>& vars)
{
    for (const Z::Variable& var : vars)
        for (ElemItemData* data : _itemsData)
            if (data->element == var.element)
            {
                data->item->setCheckState(Qt::Checked);
                data->range = var.range;
            }
    _elemsSelector->setCurrentRow(0);
}

QVector<Z::Variable> MultiElementRangeEd::collectVars()
{
    // Save currently editing range
    currentItemChanged(nullptr, _elemsSelector->currentItem());

    QVector<Z::Variable> vars;
    for (int i = 0; i < _elemsSelector->count(); i++)
        if (_elemsSelector->item(i)->checkState() == Qt::Checked)
        {
            int dataIndex = _elemsSelector->item(i)->data(Qt::UserRole).toInt();
            Z::Variable var;
            var.element = _itemsData.at(dataIndex)->element;
            var.range = _itemsData.at(dataIndex)->range;
            vars.append(var);
        }
    return vars;
}

WidgetResult MultiElementRangeEd::verify()
{
    return WidgetResult::ok(); // TODO
}

void MultiElementRangeEd::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (previous)
    {
        int prevIndex = previous->data(Qt::UserRole).toInt();
        _itemsData.at(prevIndex)->range = _rangeEditor->range();
    }
    if (current)
    {
        int curIndex = current->data(Qt::UserRole).toInt();
        _rangeEditor->setRange(_itemsData.at(curIndex)->range);
    }
}

void MultiElementRangeEd::itemDoubleClicked(QListWidgetItem *item)
{
    item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}

} // namespace VariableEditor
