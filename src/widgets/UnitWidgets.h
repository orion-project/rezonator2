#ifndef UNIT_WIDGETS_H
#define UNIT_WIDGETS_H

#include "../core/Units.h"

#include <QComboBox>

/**
    Combo-box displaying list of units of specific dimension.
*/
class UnitComboBox : public QComboBox
{
    Q_OBJECT

public:
    UnitComboBox(QWidget* parent = 0);
    UnitComboBox(Z::Dim dim, QWidget* parent = 0);

    void populate(Z::Dim dim);

    Z::Unit selectedUnit() const;
    void setSelectedUnit(Z::Unit unit);

signals:
    void focused(bool focus);

    /// Signal raises when user changes selection.
    /// It doesn't raise when selection is changed programmatically via @a setSelectedUnit().
    void unitChanged(Z::Unit unit);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    bool _enableChangeEvent = true;
    Z::Unit unitAt(int index) const;
};


/**
    Combo-box displaying list of available dimensions.
*/
class DimComboBox : public QComboBox
{
    Q_OBJECT

public:
    DimComboBox(QWidget* parent = 0);

    Z::Dim selectedDim() const;
    void setSelectedDim(Z::Dim dim);

private:
    void populate();

    Z::Dim dimAt(int index) const;
};

#endif // UNIT_WIDGETS_H
