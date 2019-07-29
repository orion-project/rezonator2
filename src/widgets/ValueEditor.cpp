#include "ValueEditor.h"

#include "Appearance.h"
#include "UnitWidgets.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriValueEdit.h"

ValueEditor::ValueEditor(QWidget *parent) : QWidget(parent)
{
    Ori::Layouts::LayoutH({
        _valueEditor = new Ori::Widgets::ValueEdit,
        _unitsSelector = new UnitComboBox
    }).setMargin(0).setSpacing(2).useFor(this);

    _valueEditor->setFont(Z::Gui::valueFont());
    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, &QLineEdit::textEdited, [this]{emit this->valueChanged();});

    connect(_unitsSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this]{emit this->valueChanged();});
}

void ValueEditor::setValue(const Z::Value& value)
{
    _valueEditor->setValue(value.value());
    _unitsSelector->populate(Z::Units::guessDim(value.unit()));
    _unitsSelector->setSelectedUnit(value.unit());
}

Z::Value ValueEditor::value() const
{
    return Z::Value(_valueEditor->value(), _unitsSelector->selectedUnit());
}
