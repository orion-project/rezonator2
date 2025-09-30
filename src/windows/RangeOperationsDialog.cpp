#include "RangeOperationsDialog.h"

#include "../app/Appearance.h"
#include "../app/HelpSystem.h"
#include "../core/Schema.h"
#include "../core/Elements.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriLabels.h"
#include "widgets/OriValueEdit.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>

#define ELEM_ICON_SIZE 40
#define SYMBOL_ICON_SIZE 24
#define EDITOR_WIDTH 80
#define GRID_SPACING_H 4
#define GRID_SPACING_V 16

using Ori::Widgets::ValueEdit;

namespace {

template<typename TEditor> TEditor* makeEditor(bool readOnly = false, int widthFactor = 1)
{
    auto e = new TEditor;
    e->setFixedWidth(EDITOR_WIDTH * widthFactor);
    e->setProperty("role", "value-editor");
    e->setReadOnly(readOnly);
    return e;
}
#define makeLineEdit makeEditor<QLineEdit>
#define makeValueEdit makeEditor<ValueEdit>

QLabel* makeGridLabel(const QString &text = {})
{
    auto l = new QLabel(text);
    l->setFixedWidth(2*SYMBOL_ICON_SIZE);
    return l;
}

} // namespace

//------------------------------------------------------------------------------
//                               SplitRangeDlg
//------------------------------------------------------------------------------

SplitRangeDlg::SplitRangeDlg(Schema *schema, ElementRange* srcElem) : QWidget(), _srcParam(srcElem->paramLength())
{
    _slider = new QSlider;
    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(0);
    _slider->setMaximum(_sliderMax);
    _slider->setValue(_sliderMax/2);
    _slider->setPageStep(_sliderMax/10);
    connect(_slider, &QSlider::valueChanged, this, &SplitRangeDlg::onSliderValueChanged);

    auto rangeLabel = makeLineEdit(true);
    rangeLabel->setText(srcElem->label());

    _rangeLabel1 = makeLineEdit();
    _rangeLabel1->setText(srcElem->label());

    _rangeLabel2 = makeLineEdit();
    _rangeLabel2->setText(Z::Utils::generateLabel(schema, srcElem->labelPrefix()));

    auto rangeParam = makeValueEdit(true);
    rangeParam->setValue(_srcParam->value().value());
    
    _editParam1 = makeValueEdit();
    _editParam2 = makeValueEdit();
    connect(_editParam1, &ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);
    connect(_editParam2, &ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);

    auto buttonSwap = new QPushButton;
    buttonSwap->setFlat(true);
    buttonSwap->setToolTip(tr("Swap elements"));
    buttonSwap->setIcon(QIcon(":/toolbar/move_left_right"));
    buttonSwap->setCursor(Qt::PointingHandCursor);
    connect(buttonSwap, &QPushButton::clicked, this, &SplitRangeDlg::onSwapButtonClicked);
    
    _insertPoint = new QCheckBox(tr("Insert point between ranges"));
    connect(_insertPoint, &QCheckBox::stateChanged, this, &SplitRangeDlg::onInsertLabelToggled);
    
    _pointLabel = makeEditor<QLineEdit>();
    _pointLabel->setEnabled(false);
    _pointLabel->setText(Z::Utils::generateLabel(schema, ElemPoint().labelPrefix()));

    auto l = new QGridLayout;
    l->setHorizontalSpacing(GRID_SPACING_H);
    l->setVerticalSpacing(GRID_SPACING_V);
    int row = 0;
    const auto elemIconPath = Z::Utils::elemIconPath(srcElem->type());
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 0);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/move_right", SYMBOL_ICON_SIZE), row, 1);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 2);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/plus", SYMBOL_ICON_SIZE), row, 3);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 4);
    row++;
    l->addWidget(rangeLabel, row, 0);
    l->addWidget(_rangeLabel1, row, 2);
    l->addWidget(buttonSwap, row, 3);
    l->addWidget(_rangeLabel2, row, 4);
    row++;
    l->addWidget(rangeParam, row, 0);
    l->addWidget(makeGridLabel(_srcParam->value().unit()->name()), row, 1);
    l->addWidget(_editParam1, row, 2);
    l->addWidget(makeGridLabel(), row, 3);
    l->addWidget(_editParam2, row, 4);
    
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(l);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(_slider);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(Ori::Layouts::LayoutH({_insertPoint, _pointLabel}).boxLayout());
    
    onSliderValueChanged();
}

void SplitRangeDlg::onSliderValueChanged()
{
    if (_skipSlider) return;
    const auto srcValue = _srcParam->value().value();
    const auto v = srcValue * double(_slider->value()) / _sliderMax;
    _editParam1->setValue(v);
    _editParam2->setValue(srcValue - v);
}

void SplitRangeDlg::onSwapButtonClicked()
{
    _insertAfter = !_insertAfter;

    _skipSlider = true;
    _slider->setValue(_sliderMax - _slider->value());
    _skipSlider = false;
 
    auto value2 = _editParam2->value();
    _editParam2->setValue(_editParam1->value());
    _editParam1->setValue(value2);
 
    auto label2 = _rangeLabel2->text();
    _rangeLabel2->setText(_rangeLabel1->text());
    _rangeLabel1->setText(label2);
}

void SplitRangeDlg::onParamValueEdited()
{
    _skipSlider = true;
    auto edit = (Ori::Widgets::ValueEdit*)sender();
    auto value = edit->value();
    const auto max = _srcParam->value().value();
    if (value < 0) {
        value = 0;
        edit->setValue(value);
    } else if (value > max) {
        value = max;
        edit->setValue(value);
    }
    auto otherValue = max - value;
    if (edit == _editParam1) {
        _editParam2->setValue(otherValue);
        otherValue = _insertAfter ? value : otherValue;
    } else {
        _editParam1->setValue(otherValue);
        otherValue = _insertAfter ? otherValue : value;
    }
    _slider->setValue(qRound(otherValue / max * _sliderMax));
    _skipSlider = false;
}

void SplitRangeDlg::onInsertLabelToggled()
{
    _pointLabel->setEnabled(_insertPoint->isChecked());
}

QString SplitRangeDlg::oldLabel() const
{
    return (_insertAfter ? _rangeLabel1 : _rangeLabel2)->text().trimmed();
}

QString SplitRangeDlg::newLabel() const
{
    return (_insertAfter ? _rangeLabel2 : _rangeLabel1)->text().trimmed();
}

Z::Value SplitRangeDlg::oldValue() const
{
    return Z::Value((_insertAfter ? _editParam1 : _editParam2)->value(), _srcParam->value().unit());
}

Z::Value SplitRangeDlg::newValue() const
{
    return Z::Value((_insertAfter ? _editParam2 : _editParam1)->value(), _srcParam->value().unit());
}

bool SplitRangeDlg::insertPoint() const
{
    return _insertPoint->isChecked();
}

QString SplitRangeDlg::pointLabel() const
{
    return _pointLabel->text().trimmed();
}

bool SplitRangeDlg::exec()
{
    return Ori::Dlg::Dialog(this, false)
        .withTitle(tr("Split"))
        .withOnHelp([]{ Z::HelpSystem::topic("elem_opers_split"); })
        .withActiveWidget(_slider)
        .exec();
}

//------------------------------------------------------------------------------
//                               MergeRangesDlg
//------------------------------------------------------------------------------

MergeRangesDlg::MergeRangesDlg(ElementRange* elem1, ElementRange* elem2) : QWidget()
{
    auto srcLabel1 = makeLineEdit(true);
    auto srcLabel2 = makeLineEdit(true);
    auto tgtLabel = makeLineEdit(true);
    
    auto srcParam1 = makeValueEdit(true);
    auto srcParam2 = makeValueEdit(true);
    auto tgtParam = makeValueEdit(true);

    auto unitLabel = makeGridLabel();
    auto spacerLabel = makeGridLabel();

    auto l = new QGridLayout(this);
    l->setHorizontalSpacing(GRID_SPACING_H);
    l->setVerticalSpacing(GRID_SPACING_V);
    int row = 0;
    const auto elemIconPath = Z::Utils::elemIconPath(elem1->type());
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 0);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/plus", SYMBOL_ICON_SIZE), row, 1);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 2);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/move_right", SYMBOL_ICON_SIZE), row, 3);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, ELEM_ICON_SIZE), row, 4);
    row++;
    l->addWidget(srcLabel1, row, 0);
    l->addWidget(srcLabel2, row, 2);
    l->addWidget(tgtLabel, row, 4);
    row++;
    l->addWidget(srcParam1, row, 0);
    l->addWidget(unitLabel, row, 1);
    l->addWidget(srcParam2, row, 2);
    l->addWidget(spacerLabel, row, 3);
    l->addWidget(tgtParam, row, 4);

    srcLabel1->setText(elem1->label());
    srcLabel2->setText(elem2->label());
    tgtLabel->setText(elem1->label());
    auto v1 = elem1->paramLength()->value();
    auto v2 = elem2->paramLength()->value();
    auto unit = v1.unit();
    unitLabel->setText(unit->name());
    srcParam1->setValue(v1.value());
    srcParam2->setValue(v2.toUnit(unit).value());
    tgtParam->setValue(Z::Value::fromSi(v1.toSi() + v2.toSi(), unit).value());
}

bool MergeRangesDlg::exec()
{
    return Ori::Dlg::Dialog(this, false)
        .withTitle(tr("Merge"))
        .withOnHelp([]{ Z::HelpSystem::topic("elem_opers_merge"); })
        .withContentToButtonsSpacingFactor(2)
        .exec();
}

//------------------------------------------------------------------------------
//                               SlideRangesDlg
//------------------------------------------------------------------------------

SlideRangesDlg::SlideRangesDlg(ElementRange* elem1, ElementRange* elem2) :
    _param1(elem1->paramLength()), _param2(elem2->paramLength())
{
    auto unit = _param1->value().unit();
    double v1 = _param1->value().value();
    double v2 = _param2->value().toUnit(unit).value();
    _valueSum = v1 + v2;

    _slider = new QSlider;
    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(0);
    _slider->setMaximum(_sliderMax);
    _slider->setPageStep(_sliderMax/10);
    _slider->setValue(qRound(v1 / _valueSum * _sliderMax));
    connect(_slider, &QSlider::valueChanged, this, &SlideRangesDlg::onSliderValueChanged);
    
    const int widthFactor = 2;

    auto label1 = new QLabel(elem1->label());
    label1->setFont(Z::Gui::ElemLabelFont().get());
    label1->setFixedWidth(EDITOR_WIDTH*widthFactor);
    label1->setAlignment(Qt::AlignHCenter);

    auto label2 = new QLabel(elem2->label());
    label2->setFont(Z::Gui::ElemLabelFont().get());
    label2->setFixedWidth(EDITOR_WIDTH*widthFactor);
    label2->setAlignment(Qt::AlignHCenter);
    
    auto srcUnit = new QLabel;
    srcUnit->setFixedWidth(EDITOR_WIDTH*widthFactor);
    srcUnit->setText(unit->name());
    srcUnit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    auto srcParam1 = makeValueEdit(true, widthFactor);
    auto srcParam2 = makeValueEdit(true, widthFactor);
    
    _editParam1 = makeValueEdit(false, widthFactor);
    _editParam2 = makeValueEdit(false, widthFactor);
    connect(_editParam1, &ValueEdit::valueEdited, this, &SlideRangesDlg::onParamValueEdited);
    connect(_editParam2, &ValueEdit::valueEdited, this, &SlideRangesDlg::onParamValueEdited);

    auto buttonSwap = new QPushButton;
    buttonSwap->setFlat(true);
    buttonSwap->setToolTip(tr("Swap lengths"));
    buttonSwap->setIcon(QIcon(":/toolbar/move_left_right"));
    buttonSwap->setCursor(Qt::PointingHandCursor);
    connect(buttonSwap, &QPushButton::clicked, this, &SlideRangesDlg::onSwapButtonClicked);

    auto l = new QGridLayout;
    l->setVerticalSpacing(10);
    int row = 0;
    l->addWidget(label1, row, 0);
    l->addWidget(label2, row, 2);
    row++;
    l->addWidget(srcParam1, row, 0);
    l->addWidget(srcUnit, row, 1);
    l->addWidget(srcParam2, row, 2);
    row++;
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/move_down", SYMBOL_ICON_SIZE), row, 0);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/move_down", SYMBOL_ICON_SIZE), row, 2);
    row++;
    l->addWidget(_editParam1, row, 0);
    l->addWidget(buttonSwap, row, 1);
    l->addWidget(_editParam2, row, 2);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(l);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(_slider);

    srcParam1->setValue(v1);
    srcParam2->setValue(v2);
    _editParam1->setValue(v1);
    _editParam2->setValue(v2);
}

void SlideRangesDlg::onSliderValueChanged()
{
    if (_skipSlider) return;
    const double v1 = double(_slider->value()) / _sliderMax * _valueSum;
    const double v2 = _valueSum - v1;
    _editParam1->setValue(v1);
    _editParam2->setValue(v2);
}

void SlideRangesDlg::onSwapButtonClicked()
{
    _skipSlider = true;
    auto v2 = _editParam2->value();
    _editParam2->setValue(_editParam1->value());
    _editParam1->setValue(v2);
    _slider->setValue(_sliderMax - _slider->value());
    _skipSlider = false;
}

void SlideRangesDlg::onParamValueEdited()
{
    _skipSlider = true;
    auto ed = qobject_cast<ValueEdit*>(sender());
    auto v = ed->value();
    if (v < 0) {
        v = 0;
        ed->setValue(v);
    } else if (v > _valueSum) {
        v = _valueSum;
        ed->setValue(v);
    }
    if (ed == _editParam1) {
        _editParam2->setValue(_valueSum - v);
    } else {
        v = _valueSum - v;
        _editParam1->setValue(v);
    }
    _slider->setValue(qRound(v / _valueSum * _sliderMax));
    _skipSlider = false;
}

bool SlideRangesDlg::exec()
{
    return Ori::Dlg::Dialog(this, false)
        .withTitle(tr("Slide"))
        .withOnHelp([]{ Z::HelpSystem::topic("elem_opers_slide"); })
        .withContentToButtonsSpacingFactor(2)
        .withActiveWidget(_slider)
        .exec();
}
