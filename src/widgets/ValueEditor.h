#ifndef VALUE_EDITOR_H
#define VALUE_EDITOR_H

#include <QWidget>

#include "../core/Values.h"

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}

class UnitComboBox;

class ValueEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ValueEditor(QWidget *parent = nullptr);

    void setValue(const Z::Value& value);
    Z::Value value() const;

signals:
    void valueChanged();

private:
    Ori::Widgets::ValueEdit* _valueEditor;
    UnitComboBox* _unitsSelector;
};

#endif // VALUE_EDITOR_H
