#include "RangeOperationsDialog.h"

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

SplitRangeDlg::SplitRangeDlg(Schema *schema, ElementRange* srcElem) : QWidget(), srcParam(srcElem->paramLength())
{
    _slider = new QSlider;
    _slider->setOrientation(Qt::Horizontal);
    _slider->setMinimum(0);
    _slider->setMaximum(sliderMax);
    _slider->setValue(sliderMax/2);
    _slider->setPageStep(sliderMax/10);
    connect(_slider, &QSlider::valueChanged, this, &SplitRangeDlg::onSliderValueChanged);

    auto rangeLabel = makeEditor<QLineEdit>();
    rangeLabel->setText(srcElem->label());
    rangeLabel->setReadOnly(true);

    _rangeLabel1 = makeEditor<QLineEdit>();
    _rangeLabel1->setText(srcElem->label());

    _rangeLabel2 = makeEditor<QLineEdit>();
    _rangeLabel2->setText(Z::Utils::generateLabel(schema, srcElem->labelPrefix()));

    auto rangeParam = makeEditor<Ori::Widgets::ValueEdit>();
    rangeParam->setValue(srcParam->value().value());
    rangeParam->setReadOnly(true);
    
    auto unitLabel = new QLabel(srcParam->value().unit()->name());
    unitLabel->setFixedWidth(2*symbolIconSize);

    auto spacerLabel = new QLabel;
    spacerLabel->setFixedWidth(2*symbolIconSize);

    _editParam1 = makeEditor<Ori::Widgets::ValueEdit>();
    _editParam2 = makeEditor<Ori::Widgets::ValueEdit>();
    connect(_editParam1, &Ori::Widgets::ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);
    connect(_editParam2, &Ori::Widgets::ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);

    auto buttonSwap = new QPushButton;
    buttonSwap->setFlat(true);
    buttonSwap->setToolTip(tr("Swap elements"));
    buttonSwap->setIcon(QIcon(":/toolbar/equ_swap"));
    connect(buttonSwap, &QPushButton::clicked, this, &SplitRangeDlg::onSwapButtonClicked);
    
    _insertPoint = new QCheckBox(tr("Insert point between ranges"));
    connect(_insertPoint, &QCheckBox::stateChanged, this, &SplitRangeDlg::onInsertLabelToggled);
    
    _pointLabel = makeEditor<QLineEdit>();
    _pointLabel->setEnabled(false);
    _pointLabel->setText(Z::Utils::generateLabel(schema, ElemPoint().labelPrefix()));

    auto l = new QGridLayout;
    l->setHorizontalSpacing(4);
    l->setVerticalSpacing(16);
    int row = 0;
    const auto elemIconPath = Z::Utils::elemIconPath(srcElem->type());
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, elemIconSize), row, 0);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/move_right", symbolIconSize), row, 1);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, elemIconSize), row, 2);
    l->addWidget(Ori::Widgets::iconLabel(":/toolbar/plus", symbolIconSize), row, 3);
    l->addWidget(Ori::Widgets::iconLabel(elemIconPath, elemIconSize), row, 4);
    row++;
    l->addWidget(rangeLabel, row, 0);
    l->addWidget(_rangeLabel1, row, 2);
    l->addWidget(buttonSwap, row, 3);
    l->addWidget(_rangeLabel2, row, 4);
    row++;
    l->addWidget(rangeParam, row, 0);
    l->addWidget(unitLabel, row, 1);
    l->addWidget(_editParam1, row, 2);
    l->addWidget(spacerLabel, row, 3);
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
    const auto srcValue = srcParam->value().value();
    const auto v = srcValue * double(_slider->value())/sliderMax;
    _editParam1->setValue(v);
    _editParam2->setValue(srcValue - v);
}

void SplitRangeDlg::onSwapButtonClicked()
{
    _insertAfter = !_insertAfter;

    _skipSlider = true;
    _slider->setValue(sliderMax - _slider->value());
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
    qDebug() << "value edited";
    _skipSlider = true;
    auto edit = (Ori::Widgets::ValueEdit*)sender();
    auto value = edit->value();
    const auto max = srcParam->value().value();
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
    _slider->setValue(qRound(otherValue / max * sliderMax));
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
    return Z::Value((_insertAfter ? _editParam1 : _editParam2)->value(), srcParam->value().unit());
}

Z::Value SplitRangeDlg::newValue() const
{
    return Z::Value((_insertAfter ? _editParam2 : _editParam1)->value(), srcParam->value().unit());
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
        .withTitle(tr("Split Range"))
        .withOnHelp([]{ Z::HelpSystem::topic("elem_opers_split_range"); })
        .withActiveWidget(_slider)
        .exec();
}
