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

class Element;
class Schema;

class SplitRangeDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit SplitRangeDlg(Schema *schema, Element *srcElem);
    
    bool exec();
    
    QString oldLabel() const;
    QString newLabel() const;
    QString pointLabel() const;
    Z::Value oldValue() const;
    Z::Value newValue() const;
    bool insertAfter() const { return _insertAfter; }
    bool insertPoint() const;
    
    static QString helpTopic() { return "elem_opers_split"; }
    
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
    explicit MergeRangesDlg(Element* elem1, Element* elem2);
    
    bool exec();
    
    static QString helpTopic() { return "elem_opers_merge"; }
};


class SlideRangesDlg : public QWidget
{
    Q_OBJECT
    
public:
    explicit SlideRangesDlg(Element* elem1, Element* elem2);
    
    bool exec();

    Z::Value value1() const;
    Z::Value value2() const;
    
    static QString helpTopic() { return "elem_opers_slide"; }
    
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
