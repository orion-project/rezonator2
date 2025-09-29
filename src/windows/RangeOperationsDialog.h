#ifndef RANGE_OPERATIONS_DIALOG_H
#define RANGE_OPERATIONS_DIALOG_H

#include "../core/Parameters.h"

#include <QWidget>

class QCheckBox;
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
    const Z::Parameter *srcParam;
    const double sliderMax = 100.0;
    const int elemIconSize = 40;
    const int symbolIconSize = 24;
    const int editorWidth = 80;
    
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

    template<typename TEditor> TEditor* makeEditor()
    {
        auto e = new TEditor;
        e->setFixedWidth(editorWidth);
        e->setProperty("role", "value-editor");
        return e;
    }
};

#endif // RANGE_OPERATIONS_DIALOG_H
