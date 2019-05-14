#ifndef UNIT_WIDGETS_H
#define UNIT_WIDGETS_H

#include "../core/Units.h"

#include <QComboBox>

QT_BEGIN_NAMESPACE
class QActionGroup;
class QMenu;
QT_END_NAMESPACE

/**
    Combo-box displaying list of units of specific dimension.
*/
class UnitComboBox : public QComboBox
{
    Q_OBJECT

public:
    UnitComboBox(QWidget* parent = nullptr);
    UnitComboBox(Z::Dim dim, QWidget* parent = nullptr);

    Z::Unit selectedUnit() const;
    void setSelectedUnit(Z::Unit unit);

    void setEnabled(bool on);

    /// If the user can select a unit from Z::Dims::fixed() dimension.
    /// It is used for 'design parameter' mode, @see SchemaParamsWindow.
    /// Call it as a part of control initialization before `populate()`.
    bool canSelectFixedUnit = false;

public slots:
    void populate(Z::Dim dim);

signals:
    void focused(bool focus);

    /// Signal raises when the user changes selection.
    /// It doesn't raise when selection is changed programmatically via @a setSelectedUnit().
    void unitChanged(Z::Unit unit);

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

private:
    bool _enableChangeEvent = true;
    bool _isEmptyOrSingleItem = true;
    Z::Unit unitAt(int index) const;
};


/**
    Combo-box displaying list of available dimensions.
*/
class DimComboBox : public QComboBox
{
    Q_OBJECT

public:
    DimComboBox(QWidget* parent = nullptr);

    Z::Dim selectedDim() const;
    void setSelectedDim(Z::Dim dim);

signals:
    /// Signal raises when the user changes selection.
    /// It doesn't raise when selection is changed programmatically via @a setSelectedDim().
    void dimChanged(Z::Dim dim);

private:
    void populate();

    Z::Dim dimAt(int index) const;

    bool _enableChangeEvent = true;
};


class UnitsMenu : public QObject
{
    Q_OBJECT

public:
    UnitsMenu(QObject *parent = nullptr);
    ~UnitsMenu();

    QMenu* menu() const { return _menu; }

    void setUnit(Z::Unit unit);

signals:
    void unitChanged(Z::Unit unit);

private:
    QMenu* _menu;
    QActionGroup* _actions;
    Z::Dim _dim = nullptr;
    Z::Unit _unit = nullptr;

    void populate();
    void actionTriggered();
};

#endif // UNIT_WIDGETS_H
