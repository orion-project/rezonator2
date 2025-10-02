#ifndef RANGE_OPERATIONS_DIALOG_H
#define RANGE_OPERATIONS_DIALOG_H

#include "../core/Parameters.h"

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;
class QSlider;

namespace Ori::Widgets {
class ValueEdit;
}

class ElementRange;
class Schema;

class SplitRangeDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit SplitRangeDlg(Schema *schema, ElementRange* srcElem);
    
    bool exec();
    
    QString oldLabel() const;
    QString newLabel() const;
    QString pointLabel() const;
    Z::Value oldValue() const;
    Z::Value newValue() const;
    bool insertAfter() const { return _insertAfter; }
    bool insertPoint() const;
    
private:
    const Z::Parameter *_srcParam;
    const double _sliderMax = 100;
    QSlider *_slider;
    QLineEdit *_rangeLabel1, *_rangeLabel2, *_pointLabel;
    Ori::Widgets::ValueEdit *_editParam1, *_editParam2;
    QSharedPointer<QWidget> _content;
    QCheckBox *_insertPoint;
    bool _insertAfter = true;
    bool _skipSlider = false;
    
    void onSliderValueChanged();
    void onSwapButtonClicked();
    void onParamValueEdited();
    void onInsertLabelToggled();
};


class MergeRangesDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit MergeRangesDlg(ElementRange* elem1, ElementRange* elem2);
    
    bool exec();
};


class SlideRangesDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit SlideRangesDlg(ElementRange* elem1, ElementRange* elem2);
    
    bool exec();

    Z::Value value1() const;
    Z::Value value2() const;
    
private:
    const Z::Parameter *_param1, *_param2;
    const double _sliderMax = 1000;
    double _valueSum;
    QSlider *_slider;
    Ori::Widgets::ValueEdit *_editParam1, *_editParam2;
    bool _skipSlider = false;

    void onSliderValueChanged();
    void onSwapButtonClicked();
    void onParamValueEdited();
};

#endif // RANGE_OPERATIONS_DIALOG_H
