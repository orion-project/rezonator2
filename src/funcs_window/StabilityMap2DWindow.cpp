#include "StabilityMap2DWindow.h"

#include "../CustomPrefs.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/Plot.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

//------------------------------------------------------------------------------
//                            StabilityMap2DParamsDlg
//------------------------------------------------------------------------------

StabilityMap2DParamsDlg::StabilityMap2DParamsDlg(Schema *schema, Z::Variable *var1, Z::Variable *var2)
    : RezonatorDialog(DontDeleteOnClose), _schema(schema)
{
    setWindowTitle(tr("2D Stability Map Parameters"));
    setObjectName("StabilityMap2DPropsDlg");

    _editor1.var = var1;
    _editor2.var = var2;

    makeControls(tr("Variable 1 (X)"), schema, &_editor1);
    makeControls(tr("Variable 2 (Y)"), schema, &_editor2);

    mainLayout()->addLayout(Ori::Layouts::LayoutH({_editor1.groupBox, _editor2.groupBox}).boxLayout());
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();

    populate();
}

void StabilityMap2DParamsDlg::makeControls(const QString &title, Schema* schema, VarEditor* editor)
{
    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>());

    editor->elemSelector = new ElemAndParamSelector(schema, elemFilter.get(), Z::Utils::defaultParamFilter());
    connect(editor->elemSelector, &ElemAndParamSelector::selectionChanged, [this, editor]{ this->guessRange(editor); });

    editor->rangeEditor = new GeneralRangeEditor;

    editor->groupBox = new QGroupBox(title);

    Ori::Layouts::LayoutV({
        editor->elemSelector,
        Ori::Layouts::Space(8),
        Ori::Gui::group(tr("Variation"), editor->rangeEditor),
        Ori::Layouts::Stretch()
    }).useFor(editor->groupBox);
}

void StabilityMap2DParamsDlg::populate()
{
    if (!_editor1.var->element || !_editor2.var->element)
    {
        auto recentObj = CustomPrefs::recentObj(_recentKey);
        Z::IO::Json::readVariablePref(recentObj["var1"].toObject(), _editor1.var, _schema);
        Z::IO::Json::readVariablePref(recentObj["var2"].toObject(), _editor2.var, _schema);
    }
    populate(&_editor1);
    populate(&_editor2);
}

void StabilityMap2DParamsDlg::populate(VarEditor* editor)
{
    editor->elemSelector->setSelectedElement(editor->var->element);
    editor->elemSelector->setSelectedParameter(editor->var->parameter);
    editor->rangeEditor->setRange(editor->var->range);
}

void StabilityMap2DParamsDlg::collect(VarEditor* editor, Z::Variable *var)
{
    var->element = editor->elemSelector->selectedElement();
    var->parameter = editor->elemSelector->selectedParameter();
    var->range = editor->rangeEditor->range();
}

void StabilityMap2DParamsDlg::collect()
{
    auto res = _editor1.elemSelector->verify();
    if (!res) return res.show(this);

    res = _editor1.rangeEditor->verify();
    if (!res) return res.show(this);

    res = _editor2.elemSelector->verify();
    if (!res) return res.show(this);

    res = _editor2.rangeEditor->verify();
    if (!res) return res.show(this);

    Z::Variable tmp1, tmp2;
    collect(&_editor1, &tmp1);
    collect(&_editor2, &tmp2);
    if (tmp1.element == tmp2.element && tmp1.parameter == tmp2.parameter)
        return Ori::Dlg::warning(tr("X-variation can't be the same as Y"));

    collect(&_editor1, _editor1.var);
    collect(&_editor2, _editor2.var);

    accept();

    CustomPrefs::setRecentObj(_recentKey, QJsonObject({
        { "var1", Z::IO::Json::writeVariablePref(_editor1.var) },
        { "var2", Z::IO::Json::writeVariablePref(_editor2.var) },
    }));
}

void StabilityMap2DParamsDlg::guessRange(VarEditor* editor)
{
    auto param = editor->elemSelector->selectedParameter();
    if (!param) return;

    // TODO restore or guess range limits and step
    Z::VariableRange range;
    range.start = param->value();
    range.stop = param->value();
    range.step = param->value() * 0;
    editor->rangeEditor->setRange(range);
}

//------------------------------------------------------------------------------
//                             StabilityMap2DWindow
//------------------------------------------------------------------------------

StabilityMap2DWindow::StabilityMap2DWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMap2DFunction(schema))
{
    _graphT = new QCPColorMap(_plot->xAxis, _plot->yAxis);

    _colorScale = new QCPColorScale(_plot);
    auto colorAxis = _colorScale->axis();
    colorAxis->setLabel(tr("Stability Parameter"));
    colorAxis->setLabelFont(_plot->xAxis->labelFont());
    colorAxis->setSelectedLabelFont(_plot->xAxis->selectedLabelFont());
    _plot->plotLayout()->addElement(_plot->axisRectRow(), _plot->axisRectCol() + 1, _colorScale);

    _plot->useSafeMargins = false;
    // We have to do this way because QCPColorMap::rescaleAxes() seems not working as expected
    _plot->excludeServiceGraphsFromAutolimiting = false;

    _graphT->setColorScale(_colorScale);
    _graphT->setGradient(QCPColorGradient::gpJet);
    _graphT->setSelectable(QCP::stNone);

    // Make sure the axis rect and color scale synchronize their bottom and top margins:
    QCPMarginGroup *marginGroup = new QCPMarginGroup(_plot);
    _plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    _colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    function()->graphT = _graphT;

    createControl();
}

void StabilityMap2DWindow::createControl()
{
    _actnStabilityAutolimits = new QAction(tr("Z-axis -> Stability Range", "Plot action"), this);
    _actnStabilityAutolimits->setIcon(QIcon(":/toolbar/limits_stab"));
    connect(_actnStabilityAutolimits, &QAction::triggered, [this](){autolimitsStability(true);});

    menuLimits->addSeparator();
    menuLimits->addAction(_actnStabilityAutolimits);

    toolbar()->addSeparator();
    toolbar()->addAction(_actnStabilityAutolimits);
}


bool StabilityMap2DWindow::configureInternal()
{
    return StabilityMap2DParamsDlg(schema(), function()->paramX(), function()->paramY()).run();
}

ElemDeletionReaction StabilityMap2DWindow::reactElemDeletion(const Elements& elems)
{
    if (elems.contains(function()->paramX()->element) or
        elems.contains(function()->paramY()->element))
        return ElemDeletionReaction::Close;
    return ElemDeletionReaction::None;
}

void StabilityMap2DWindow::elementDeleting(Schema*, Element* elem)
{
    if (function()->paramX()->element == elem or
        function()->paramY()->element == elem)
        disableAndClose();
}

void StabilityMap2DWindow::updateGraphs()
{
    if (_isFirstCalc)
    {
        autolimitsStability(false);
        _isFirstCalc = false;
    }
}

QString StabilityMap2DWindow::getDefaultTitle() const
{
    return tr("2D Stability Map");
}

namespace  {
QString getDefaultAxisTitle(const Z::Variable* arg, Z::Unit unit)
{
    if (unit == Z::Units::none())
        return QStringLiteral("%1, %2")
                .arg(arg->element->displayLabelTitle())
                .arg(arg->parameter->name());
    return QStringLiteral("%1, %2 (%3)")
            .arg(arg->element->displayLabelTitle())
            .arg(arg->parameter->label())
            .arg(unit->name());
}
} // namespace

QString StabilityMap2DWindow::getDefaultTitleX() const
{
    return getDefaultAxisTitle(function()->paramX(), getUnitX());
}

QString StabilityMap2DWindow::getDefaultTitleY() const
{
    return getDefaultAxisTitle(function()->paramY(), getUnitY());
}

void StabilityMap2DWindow::autolimitsStability(bool replot)
{
    switch (function()->stabilityCalcMode())
    {
    case Z::Enums::StabilityCalcMode::Normal:
        _colorScale->axis()->setRange(-1.05, 1.05);
        break;

    case Z::Enums::StabilityCalcMode::Squared:
        _colorScale->axis()->setRange(-0.05, 1.05);
        break;
    }
    if (replot) _plot->replot();
}
