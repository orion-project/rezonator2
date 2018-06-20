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
    The whole variable range editor that allows to assign
    a starting and an ending values of the range and a number of points.
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
    The variable range editor that only allows to set a number of points,
    but not whether a starting and an ending values.
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
};

} // namespace VariableRangeEditor

#endif // VARIABLE_RANGE_WIDGET_H
