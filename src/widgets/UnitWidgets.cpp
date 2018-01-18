#include "Appearance.h"
#include "UnitWidgets.h"

UnitComboBox::UnitComboBox(QWidget* parent) : QComboBox(parent)
{
    setEnabled(false);
    setFixedWidth(Z::Gui::unitsSelectorWidth());
}

UnitComboBox::UnitComboBox(Z::Dim dim, QWidget* parent) : UnitComboBox(parent)
{
    populate(dim);
}

void UnitComboBox::populate(Z::Dim dim)
{
    clear();
    setEnabled(dim != Z::Dims::none());
    for (auto unit: dim->units())
        addItem(unit->name(), qVariantFromValue((void*)unit));
}

Z::Unit UnitComboBox::selectedUnit() const
{
    return unitAt(currentIndex());
}

void UnitComboBox::setSelectedUnit(Z::Unit unit)
{
    for (int i = 0; i < count(); i++)
        if (unitAt(i) == unit)
        {
            setCurrentIndex(i);
            return;
        }
}

Z::Unit UnitComboBox::unitAt(int index) const
{
    return (Z::Unit)itemData(index).value<void*>();
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

//------------------------------------------------------------------------------

DimComboBox::DimComboBox(QWidget* parent) : QComboBox(parent)
{
    populate();
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
