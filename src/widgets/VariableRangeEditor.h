#ifndef VARIABLE_RANGE_EDITOR_H
#define VARIABLE_RANGE_EDITOR_H

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

/**
    The full variable range editor that allows to assign
     starting and ending value of the range and the number of points.
*/
class GeneralRangeEditor : public QGridLayout
{
    Q_OBJECT

public:
    GeneralRangeEditor();

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
class PointsRangeEditor : public QGridLayout
{
    Q_OBJECT

public:
    PointsRangeEditor();

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

#endif // VARIABLE_RANGE_EDITOR_H
