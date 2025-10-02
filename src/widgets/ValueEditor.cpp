#include "ValueEditor.h"

#include "UnitWidgets.h"
#include "../app/Appearance.h"

#include "helpers/OriLayouts.h"
#include "widgets/OriValueEdit.h"

ValueEditor::ValueEditor(QWidget *parent) : QWidget(parent)
{
    Ori::Layouts::LayoutH({
        _valueEditor = new Ori::Widgets::ValueEdit,
        _unitsSelector = new UnitComboBox
    }).setMargin(0).setSpacing(2).useFor(this);

    _valueEditor->setFont(Z::Gui::ValueFont().get());
    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, &QLineEdit::textEdited, this, [this]{emit this->valueChanged();});

    connect(_unitsSelector, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, [this]{emit this->valueChanged();});
}

void ValueEditor::setValue(const Z::Value& value, Z::Dim dim)
{
    _valueEditor->setValue(value.value());
    _unitsSelector->populate(dim ? dim : Z::Units::guessDim(value.unit()));
    if (allowPercent)
        _unitsSelector->addUnit(Z::Units::percent());
    _unitsSelector->setSelectedUnit(value.unit());
}

Z::Value ValueEditor::value() const
{
    return Z::Value(_valueEditor->value(), _unitsSelector->selectedUnit());
}
