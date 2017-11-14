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

class VariableRangeWidget : public QGridLayout
{
    Q_OBJECT

public:
    VariableRangeWidget();

    Z::VariableRange range();
    void setRange(const Z::VariableRange& var);

    WidgetResult verify();

private:
    QSpinBox *sePoints;
    QRadioButton *rbStep, *rbPoints;
    ValueEditor *edStart, *edStop, *edStep;
};


class VariableRangeWidget_ElementRange : public QGridLayout
{
    Q_OBJECT

public:
    VariableRangeWidget_ElementRange();

    Z::VariableRange range();
    void setRange(const Z::VariableRange& var);

    WidgetResult verify();

private:
    Z::Value _start, _stop;
    QSpinBox *sePoints;
    QRadioButton *rbStep, *rbPoints;
    ValueEditor *edStep;
};

#endif // VARIABLE_RANGE_WIDGET_H
