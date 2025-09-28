#include "RangeOperationsDialog.h"

#include "../core/Schema.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriLabels.h"
#include "widgets/OriValueEdit.h"

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

    auto editLabel = makeEditor<QLineEdit>();
    editLabel->setText(srcElem->label());
    editLabel->setReadOnly(true);

    _editLabel1 = makeEditor<QLineEdit>();
    _editLabel1->setText(srcElem->label());

    _editLabel2 = makeEditor<QLineEdit>();
    _editLabel2->setText(Z::Utils::generateLabel(schema, srcElem->labelPrefix()));

    auto editParam = makeEditor<Ori::Widgets::ValueEdit>();
    editParam->setValue(srcParam->value().value());
    editParam->setReadOnly(true);
    
    auto labelUnit = new QLabel(srcParam->value().unit()->name());
    labelUnit->setFixedWidth(2*symbolIconSize);

    auto labelSpacer = new QLabel;
    labelSpacer->setFixedWidth(2*symbolIconSize);

    _editParam1 = makeEditor<Ori::Widgets::ValueEdit>();
    _editParam2 = makeEditor<Ori::Widgets::ValueEdit>();
    connect(_editParam1, &Ori::Widgets::ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);
    connect(_editParam2, &Ori::Widgets::ValueEdit::valueEdited, this, &SplitRangeDlg::onParamValueEdited);

    auto buttonSwap = new QPushButton;
    buttonSwap->setFlat(true);
    buttonSwap->setToolTip(tr("Swap elements"));
    buttonSwap->setIcon(QIcon(":/toolbar/equ_swap"));
    connect(buttonSwap, &QPushButton::clicked, this, &SplitRangeDlg::onSwapButtonClicked);

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
    l->addWidget(editLabel, row, 0);
    l->addWidget(_editLabel1, row, 2);
    l->addWidget(buttonSwap, row, 3);
    l->addWidget(_editLabel2, row, 4);
    row++;
    l->addWidget(editParam, row, 0);
    l->addWidget(labelUnit, row, 1);
    l->addWidget(_editParam1, row, 2);
    l->addWidget(labelSpacer, row, 3);
    l->addWidget(_editParam2, row, 4);
    
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(l);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(_slider);
    
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
 
    auto label2 = _editLabel2->text();
    _editLabel2->setText(_editLabel1->text());
    _editLabel1->setText(label2);
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

QString SplitRangeDlg::oldLabel() const
{
    return (_insertAfter ? _editLabel1 : _editLabel2)->text().trimmed();
}

QString SplitRangeDlg::newLabel() const
{
    return (_insertAfter ? _editLabel2 : _editLabel1)->text().trimmed();
}

Z::Value SplitRangeDlg::oldValue() const
{
    return Z::Value((_insertAfter ? _editParam1 : _editParam2)->value(), srcParam->value().unit());
}

Z::Value SplitRangeDlg::newValue() const
{
    return Z::Value((_insertAfter ? _editParam2 : _editParam1)->value(), srcParam->value().unit());
}

bool SplitRangeDlg::exec()
{
    return Ori::Dlg::Dialog(this, false)
        .withTitle(tr("Split Range"))
        .withActiveWidget(_slider)
        .exec();
}
