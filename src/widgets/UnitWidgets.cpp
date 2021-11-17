#include "UnitWidgets.h"

#include "../Appearance.h"

#include <QActionGroup>
#include <QDebug>
#include <QMenu>

//------------------------------------------------------------------------------
//                              UnitComboBox
//------------------------------------------------------------------------------

UnitComboBox::UnitComboBox(QWidget* parent) : QComboBox(parent)
{
    setEnabled(false);
    setFixedWidth(Z::Gui::unitsSelectorWidth());
    setFont(Z::Gui::ValueFont().get());

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index){
        if (index >= 0 && _enableChangeEvent) emit unitChanged(unitAt(index));
    });
}

UnitComboBox::UnitComboBox(Z::Dim dim, QWidget* parent) : UnitComboBox(parent)
{
    populate(dim);
}

void UnitComboBox::populate(Z::Dim dim)
{
    _enableChangeEvent = false;

    clear();
    foreach (auto unit, dim->units())
        addItem(unit->name(), QVariant::fromValue(unit));

    _isEmptyOrSingleItem = count() < 2 or
        (dim == Z::Dims::fixed() and !canSelectFixedUnit);

    _enableChangeEvent = true;
    setEnabled(true);
}

Z::Unit UnitComboBox::selectedUnit() const
{
    return unitAt(currentIndex());
}

void UnitComboBox::setSelectedUnit(Z::Unit unit)
{
    _enableChangeEvent = false;
    for (int i = 0; i < count(); i++)
        if (unitAt(i) == unit)
        {
            setCurrentIndex(i);
            break;
        }
    _enableChangeEvent = true;
}

Z::Unit UnitComboBox::unitAt(int index) const
{
    return itemData(index).value<Z::Unit>();
}

void UnitComboBox::focusInEvent(QFocusEvent *e)
{
    QComboBox::focusInEvent(e);
    emit focused(true);
}

void UnitComboBox::focusOutEvent(QFocusEvent *e)
{
    QComboBox::focusOutEvent(e);
    emit focused(false);
}

void UnitComboBox::setEnabled(bool on)
{
    QComboBox::setEnabled(!_isEmptyOrSingleItem && on);
}

//------------------------------------------------------------------------------
//                             DimComboBox
//------------------------------------------------------------------------------

DimComboBox::DimComboBox(QWidget* parent) : QComboBox(parent)
{
    populate();
    setFont(Z::Gui::ValueFont().get());

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index){
        if(index >= 0 && _enableChangeEvent) emit dimChanged(dimAt(index));
    });
}

void DimComboBox::populate()
{
    _enableChangeEvent = false;
    clear();
    foreach (auto dim, Z::Dims::dims())
        addItem(dim->name(), QVariant::fromValue(dim));
    _enableChangeEvent = true;
}

Z::Dim DimComboBox::selectedDim() const
{
    return dimAt(currentIndex());
}

void DimComboBox::setSelectedDim(Z::Dim dim)
{
    _enableChangeEvent = false;
    for (int i = 0; i < count(); i++)
        if (dimAt(i) == dim)
        {
            setCurrentIndex(i);
            break;
        }
    _enableChangeEvent = true;
}

Z::Dim DimComboBox::dimAt(int index) const
{
    return itemData(index).value<Z::Dim>();
}

//------------------------------------------------------------------------------
//                                  UnitsMenu
//------------------------------------------------------------------------------

UnitsMenu::UnitsMenu(QObject* parent) : QObject(parent)
{
    _menu = new QMenu;
    _actions = new QActionGroup(this);
    _actions->setExclusive(true);
}

UnitsMenu::~UnitsMenu()
{
    delete _menu;
}

void UnitsMenu::setUnit(Z::Unit unit)
{
    if (_unit == unit) return;

    _unit = unit;
    if (unit == Z::Units::none())
    {
        _menu->clear();
        _menu->setEnabled(false);
        return;
    }

    auto dim = Z::Units::guessDim(unit);
    if (dim == Z::Dims::fixed())
    {
        _menu->clear();
        _menu->setEnabled(false);
        return;
    }

    _menu->setEnabled(true);
    if (dim != _dim || _menu->isEmpty())
    {
        _dim = dim;
        populate();
    }
    foreach (auto action, _menu->actions())
        if (action->data().value<Z::Unit>() == unit)
        {
            action->setChecked(true);
            break;
        }
}

void UnitsMenu::populate()
{
    _menu->clear();
    foreach (auto unit, _dim->units())
    {
        auto action = _menu->addAction(unit->name());
        action->setData(QVariant::fromValue(unit));
        action->setCheckable(true);
        connect(action, &QAction::triggered, this, &UnitsMenu::actionTriggered);
        _actions->addAction(action);
    }
}

void UnitsMenu::actionTriggered()
{
    auto action = qobject_cast<QAction*>(sender());
    emit unitChanged(action->data().value<Z::Unit>());
}
