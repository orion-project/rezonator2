#ifndef UNIT_WIDGETS_H
#define UNIT_WIDGETS_H

#include "../core/Units.h"

#include <QComboBox>

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

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    Z::Unit unitAt(int index) const;
};

#endif // UNIT_WIDGETS_H
