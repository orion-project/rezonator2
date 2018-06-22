#include "VariableRangeEditor.h"

#include "Appearance.h"
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

namespace VariableRangeEditor {

static const int MIN_POINTS_COUNT = 3;
static const int MAX_POINTS_COUNT = 10000;

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

        layout = new QHBoxLayout;
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(editor);
        layout->addSpacing(Ori::Gui::borderWidth() + Z::Gui::unitsSelectorWidth());

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
//                                 GeneralRangeEd

GeneralRangeEd::GeneralRangeEd() : QGridLayout()
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

void GeneralRangeEd::setRange(const Z::VariableRange& var)
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

Z::VariableRange GeneralRangeEd::range()
{
    Z::VariableRange range;
    range.start = edStart->value();
    range.stop = edStop->value();
    range.step = edStep->value();
    range.points = sePoints->value();
    range.useStep = rbStep->isChecked();
    return range;
}

WidgetResult GeneralRangeEd::verify()
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
//                                 PointsRangeEd

PointsRangeEd::PointsRangeEd() : QGridLayout()
{
    StepEditor step;
    edStep = step.editor;
    rbStep = step.flag;

    PointsEditor points;
    sePoints = points.editor;
    rbPoints = points.flag;

    const int row0 = 0;
    const int row1 = 1;
    const int row2 = 2;
    const int col0 = 0;
    const int col1 = 1;

    auto stopValueLabel = new QLabel(tr("Plottable length:"));
    Z::Gui::setFontStyle(stopValueLabel, false, true);

    _stopValueLabel = new QLabel;
    Z::Gui::setValueFont(_stopValueLabel);
    Z::Gui::setFontStyle(_stopValueLabel, false, true);

    addWidget(stopValueLabel, row0, col0);
    addWidget(_stopValueLabel, row0, col1);
    addWidget(edStep, row1, col1); // before radio-buttons meaning tab-order
    addLayout(points.layout, row2, col1);
    addWidget(rbStep, row1, col0);
    addWidget(rbPoints, row2, col0);
}

void PointsRangeEd::setRange(const Z::VariableRange& var)
{
    _start = var.start;
    _stop = var.stop;

    _stopValueLabel->setText(_stop.displayStr());

    edStep->setValue(var.step);
    sePoints->setValue(var.points);
    if (var.useStep)
        rbStep->setChecked(true);
    else
        rbPoints->setChecked(true);
}

Z::VariableRange PointsRangeEd::range()
{
    Z::VariableRange range;
    range.start = _start;
    range.stop = _stop;
    range.step = edStep->value();
    range.points = sePoints->value();
    range.useStep = rbStep->isChecked();
    return range;
}

WidgetResult PointsRangeEd::verify()
{
    auto res = range();

    if (res.useStep && res.step <= 0)
        return WidgetResult::fail(edStep, tr("Value of <b>step</b> must be greater than zero."));

    if (res.useStep && res.step >= _stop)
        return WidgetResult::fail(edStep, tr("<b>Step</b> must be less than element length (%1).").arg(_stop.str()));

    return WidgetResult::ok();
}

} // namespace VariableRangeEditor
