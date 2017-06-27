#include "Appearance.h"
#include "VariableRangeWidget.h"
#include "ValueEditor.h"
#include "../core/Element.h"
#include "../core/Variable.h"
#include "widgets/OriValueEdit.h"
#include "helpers/OriWidgets.h"

#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>

#define MIN_POINTS_COUNT 3
#define MAX_POINTS_COUNT 10000

struct PointsEditor
{
    QSpinBox* editor;
    QBoxLayout* layout;
    QRadioButton* flag;

    PointsEditor()
    {
        flag = new QRadioButton(qApp->tr("Number of points", "Variable range editor"));

        editor = new QSpinBox;
        editor->setAlignment(Qt::AlignRight);
        editor->setRange(MIN_POINTS_COUNT, MAX_POINTS_COUNT);
        Z::Gui::setValueFont(editor);

        layout = Ori::Gui::layoutH(0, 0, {
            editor, Ori::Gui::spacing(Ori::Gui::borderWidth() + Z::Gui::unitsSelectorWidth()) });

        qApp->connect(editor, SIGNAL(valueChanged(int)), flag, SLOT(click()));
    }
};

struct StepEditor
{
    ValueEditor* editor;
    QRadioButton* flag;

    StepEditor()
    {
        editor = new ValueEditor;
        flag = new QRadioButton(qApp->tr("With step", "Variable range editor"));

        qApp->connect(editor, SIGNAL(valueChanged()), flag, SLOT(click()));
    }
};

//------------------------------------------------------------------------------
//                            VariableRangeWidget

VariableRangeWidget::VariableRangeWidget() : QGridLayout()
{
    edStart = new ValueEditor;
    edStop = new ValueEditor;
    edStep = new ValueEditor;

    StepEditor step;
    edStep = step.editor;
    rbStep = step.flag;

    PointsEditor points;
    sePoints = points.editor;
    rbPoints = points.flag;

    addWidget(new QLabel(tr("From")), 0, 0);
    addWidget(edStart, 0, 1);
    addWidget(new QLabel(tr("To")), 1, 0);
    addWidget(edStop, 1, 1);
    addWidget(edStep, 2, 1); // before radio-buttons meaning tab-order
    addLayout(points.layout, 3, 1);
    addWidget(rbStep, 2, 0);
    addWidget(rbPoints, 3, 0);
}

void VariableRangeWidget::setRange(const Z::VariableRange& var)
{
    edStart->setValue(var.start);
    edStop->setValue(var.stop);
    edStep->setValue(var.step);
    sePoints->setValue(var.points);
    if (var.useStep)
        rbStep->setChecked(true);
    else
        rbPoints->setChecked(true);
}

Z::VariableRange VariableRangeWidget::range()
{
    Z::VariableRange range;
    range.start = edStart->value();
    range.stop = edStop->value();
    range.step = edStep->value();
    range.points = sePoints->value();
    range.useStep = rbStep->isChecked();
    return range;
}

WidgetResult VariableRangeWidget::verify()
{
    auto res = range();

    if (res.start >= res.stop)
        return WidgetResult::fail(edStart, tr("<b>Starting value</b> of variable must be less than its end value."));

    if (res.useStep && res.step <= 0)
        return WidgetResult::fail(edStep, tr("Value of <b>step</b> must be greater than zero."));

    if (res.useStep && res.step >= (res.stop.toSi() - res.start.toSi()))
        return WidgetResult::fail(edStep, tr("<b>Step</b> must be less than variation range."));

    return WidgetResult::ok();
}

//------------------------------------------------------------------------------
//                       VariableRangeWidget_ElementRange

VariableRangeWidget_ElementRange::VariableRangeWidget_ElementRange() : QGridLayout()
{
    StepEditor step;
    edStep = step.editor;
    rbStep = step.flag;

    PointsEditor points;
    sePoints = points.editor;
    rbPoints = points.flag;

    addWidget(edStep, 0, 1); // before radio-buttons meaning tab-order
    addLayout(points.layout, 1, 1);
    addWidget(rbStep, 0, 0);
    addWidget(rbPoints, 1, 0);
}

void VariableRangeWidget_ElementRange::setRange(const Z::VariableRange& var)
{
    _max = var.stop;

    edStep->setValue(var.step);
    sePoints->setValue(var.points);
    if (var.useStep)
        rbStep->setChecked(true);
    else
        rbPoints->setChecked(true);
}

Z::VariableRange VariableRangeWidget_ElementRange::range()
{
    Z::VariableRange range;
    range.start = 0;
    range.stop = _max;
    range.step = edStep->value();
    range.points = sePoints->value();
    range.useStep = rbStep->isChecked();
    return range;
}

WidgetResult VariableRangeWidget_ElementRange::verify()
{
    auto res = range();

    if (res.useStep && res.step <= 0)
        return WidgetResult::fail(edStep, tr("Value of <b>step</b> must be greater than zero."));

    if (res.useStep && res.step >= _max)
        return WidgetResult::fail(edStep, tr("<b>Step</b> must be less than element length (%1).").arg(_max.str()));

    return WidgetResult::ok();
}
