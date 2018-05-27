#include "Appearance.h"
#include "UnitWidgets.h"

UnitComboBox::UnitComboBox(QWidget* parent) : QComboBox(parent)
{
    setEnabled(false);
    setFixedWidth(Z::Gui::unitsSelectorWidth());
    Z::Gui::setValueFont(this);

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
    for (auto unit: dim->units())
        addItem(unit->name(), qVariantFromValue((void*)unit));
    _isEmptyOrSingleItem = count() < 2;
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
    return reinterpret_cast<Z::Unit>(itemData(index).value<void*>());
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

DimComboBox::DimComboBox(QWidget* parent) : QComboBox(parent)
{
    populate();
    Z::Gui::setValueFont(this);
}

void DimComboBox::populate()
{
    clear();
    for (auto dim: Z::Dims::dims())
        addItem(dim->name(), qVariantFromValue((void*)dim));
}

Z::Dim DimComboBox::selectedDim() const
{
    return dimAt(currentIndex());
}

void DimComboBox::setSelectedDim(Z::Dim dim)
{
    for (int i = 0; i < count(); i++)
        if (dimAt(i) == dim)
        {
            setCurrentIndex(i);
            return;
        }
}

Z::Dim DimComboBox::dimAt(int index) const
{
    return (Z::Dim)itemData(index).value<void*>();
}
