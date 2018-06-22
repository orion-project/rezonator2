#ifndef VARIABLE_RANGE_WIDGET_H
#define VARIABLE_RANGE_WIDGET_H

#include <QGridLayout>
#include <QVBoxLayout>

#include "../core/Variable.h"
#include "WidgetResult.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QRadioButton;
class QSpinBox;
QT_END_NAMESPACE

class Element;
class ElementRange;
class Schema;
class ValueEditor;

namespace Ori {
namespace Widgets {
    class ValueEdit;
}}

namespace VariableRangeEditor {

/**
    The full variable range editor that allows to assign
     starting and ending value of the range and the number of points.
*/
class GeneralRangeEd : public QGridLayout
{
    Q_OBJECT

public:
    GeneralRangeEd();

    Z::VariableRange range();
    void setRange(const Z::VariableRange& var);

    WidgetResult verify();

private:
    QSpinBox *sePoints;
    QRadioButton *rbStep, *rbPoints;
    ValueEditor *edStart, *edStop, *edStep;
};

/**
    The variable range editor that only allows to set the number of points,
    but not starting or ending value.
*/
class PointsRangeEd : public QGridLayout
{
    Q_OBJECT

public:
    PointsRangeEd();

    Z::VariableRange range();
    void setRange(const Z::VariableRange& var);

    WidgetResult verify();

private:
    Z::Value _start, _stop;
    QSpinBox *sePoints;
    QRadioButton *rbStep, *rbPoints;
    ValueEditor *edStep;
    QLabel *_stopValueLabel;
};

} // namespace VariableRangeEditor

#endif // VARIABLE_RANGE_WIDGET_H
