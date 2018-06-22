#include "VariableEditor.h"

#include "ElementImagesProvider.h"
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

    auto param = elemRange->paramLength();
    auto unit = param->value().unit();

    // TODO restore or guess step
    Z::VariableRange range;
    range.start = Z::Value(0, unit);
    range.stop = Z::Value(unit->fromSi(elemRange->axisLengthSI()), unit);
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

struct ElemItemData
{
    ElementRange* element;
    QListWidgetItem* item;
    Z::VariableRange range;
};

MultiElementRangeEd::MultiElementRangeEd(Schema *schema) : QVBoxLayout()
{
    _elemsSelector = new QListWidget;
    _elemsSelector->addAction(action("", this, SLOT(selectAllElements()), "", Qt::CTRL+Qt::Key_A));
    _elemsSelector->addAction(action("", this, SLOT(deselectAllElements()), "", Qt::CTRL+Qt::Key_D));
    _elemsSelector->addAction(action("", this, SLOT(invertElementsSelection()), "", Qt::CTRL+Qt::Key_I));
    connect(_elemsSelector, &QListWidget::currentItemChanged, this, &MultiElementRangeEd::currentItemChanged);
    connect(_elemsSelector, &QListWidget::itemDoubleClicked, this, &MultiElementRangeEd::invertCheckState);
    connect(_elemsSelector, &QListWidget::itemClicked, [&](QListWidgetItem *item){
        if (!item->isSelected()) _elemsSelector->setCurrentItem(item); });

    auto elemsSelector = LayoutH({
        _elemsSelector,
        LayoutV({
            iconToolButton(tr("Select All"), ":/toolbar16/check_all", this, SLOT(selectAllElements())),
            iconToolButton(tr("Select None"), ":/toolbar16/check_none", this, SLOT(deselectAllElements())),
            iconToolButton(tr("Invert Selection"), ":/toolbar16/check_invert", this, SLOT(invertElementsSelection())),
            Stretch()
        })
    })
    .setMargin(0)
    .boxLayout();

    // Collect available elements data
    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());
    for (auto elem : schema->elements())
        if (elemFilter->check(elem))
        {
            auto data = new ElemItemData;
            data->element = Z::Utils::asRange(elem);
            auto unit = data->element->paramLength()->value().unit();
            data->range.stop = Z::Value(unit->fromSi(data->element->axisLengthSI()), unit);
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
        auto item = new QListWidgetItem("  " + itemData->element->displayLabelTitle());
        item->setIcon(QIcon(ElementImagesProvider::instance().iconPath(itemData->element->type())));
        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, i);
        itemData->item = item;
        _elemsSelector->addItem(item);
    }

    _sameSettings = new QCheckBox(tr("Use these settings for all elements"));

    _rangeEditor = new VariableRangeEditor::PointsRangeEd;
    _rangeEditor->addWidget(_sameSettings, _rangeEditor->rowCount()+1, 0, 1, _rangeEditor->columnCount());

    addLayout(elemsSelector);
    addSpacing(4);
    addWidget(group(tr("Plot accuracy"), _rangeEditor));
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
    saveEditedRange(_elemsSelector->currentItem());

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
    saveEditedRange(previous);
    showRangeInEditor(current);
}

void MultiElementRangeEd::showRangeInEditor(QListWidgetItem *item)
{
    if (!item) return;
    int itemIndex = item->data(Qt::UserRole).toInt();
    _rangeEditor->setRange(_itemsData.at(itemIndex)->range);
}

void MultiElementRangeEd::saveEditedRange(QListWidgetItem *item)
{
    if (!item) return;
    int itemIndex = item->data(Qt::UserRole).toInt();
    auto newRange = _rangeEditor->range();
    if (_sameSettings->isChecked())
        for (ElemItemData *itemData : _itemsData)
            itemData->range.assignPoints(newRange);
    else
        _itemsData.at(itemIndex)->range.assignPoints(newRange);
}

void MultiElementRangeEd::invertCheckState(QListWidgetItem *item)
{
    item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}

void MultiElementRangeEd::selectAllElements()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        _elemsSelector->item(i)->setCheckState(Qt::Checked);
}

void MultiElementRangeEd::deselectAllElements()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        _elemsSelector->item(i)->setCheckState(Qt::Unchecked);
}

void MultiElementRangeEd::invertElementsSelection()
{
    for (int i = 0; i < _elemsSelector->count(); i++)
        invertCheckState(_elemsSelector->item(i));
}

} // namespace VariableEditor
