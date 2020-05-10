#include "StabilityMap2DWindow.h"

#include "FuncOptionsPanel.h"
#include "../CustomPrefs.h"
#include "../core/Format.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"
#include "../widgets/PlotHelpers.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <qcpl_plot.h>

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
    QSharedPointer<ElementFilter> elemFilter(
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
//                            StabilityMapOptionsPanel
//------------------------------------------------------------------------------

class StabilityMap2DOptionsPanel : public FuncOptionsPanel
{
public:
    StabilityMap2DOptionsPanel(StabilityMap2DWindow* window);
    int currentFunctionMode() const override;
    void functionModeChanged(int mode) override;

private:
    StabilityMap2DWindow* _window;
};

StabilityMap2DOptionsPanel::StabilityMap2DOptionsPanel(StabilityMap2DWindow* window) : FuncOptionsPanel(window), _window(window)
{
    // TODO: check if these strings are translated
    Ori::Layouts::LayoutV({
        makeSectionHeader(tr("Stability parameter")),
        makeModeButton(":/toolbar/formula_stab_normal", tr("Normal"), int(Z::Enums::StabilityCalcMode::Normal)),
        makeModeButton(":/toolbar/formula_stab_squared", tr("Squared"), int(Z::Enums::StabilityCalcMode::Squared)),
        Ori::Layouts::Stretch()
    }).setSpacing(0).setMargin(0).useFor(this);

    for (auto button : _modeButtons)
        button->setIconSize(QSize(96, 48));

    showCurrentMode();
}

int StabilityMap2DOptionsPanel::currentFunctionMode() const
{
    return static_cast<int>(_window->function()->stabilityCalcMode());
}

void StabilityMap2DOptionsPanel::functionModeChanged(int mode)
{
    auto stabCalcMode = static_cast<Z::Enums::StabilityCalcMode>(mode);
    CustomPrefs::setRecentStr(QStringLiteral("func_stab_2d_map_mode"), Z::Enums::toStr(stabCalcMode));
    _window->function()->setStabilityCalcMode(stabCalcMode);
}

//------------------------------------------------------------------------------
//                             StabilityMap2DWindow
//------------------------------------------------------------------------------

StabilityMap2DWindow::StabilityMap2DWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMap2DFunction(schema))
{
    _exclusiveModeTS = true;
    actnShowFlippedTS->setVisible(false);
    actnShowFlippedTS->setEnabled(false);
    actnShowT->setChecked(true);
    actnShowS->setChecked(false);

    _plot->useSafeMargins = false;
    // We have to do this way because QCPColorMap::rescaleAxes() seems not working as expected
    _plot->excludeServiceGraphsFromAutolimiting = false;

    _autolimiter = _plot->addGraph();
    _autolimiter->setPen(QPen(Qt::transparent));

    _graph = new QCPColorMap(_plot->xAxis, _plot->yAxis);

    _colorScale = new QCPColorScale(_plot);
    auto colorAxis = _colorScale->axis();
    colorAxis->setLabel(tr("Stability Parameter"));
    colorAxis->setLabelFont(_plot->xAxis->labelFont());
    colorAxis->setSelectedLabelFont(_plot->xAxis->selectedLabelFont());
    _plot->plotLayout()->addElement(_plot->axisRectRow(), _plot->axisRectCol() + 1, _colorScale);

    _graph->setColorScale(_colorScale);
    _graph->setGradient(QCPColorGradient::gpJet);
    _graph->setSelectable(QCP::stNone);

    // Make sure the axis rect and color scale synchronize their bottom and top margins:
    QCPMarginGroup *marginGroup = new QCPMarginGroup(_plot);
    _plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    _colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    createControl();
}

void StabilityMap2DWindow::createControl()
{
    _actnStabilityAutolimits = new QAction(tr("Z-axis -> Stability Range", "Plot action"), this);
    _actnStabilityAutolimits->setIcon(QIcon(":/toolbar/limits_stab"));
    connect(_actnStabilityAutolimits, &QAction::triggered, [this](){autolimitsStability(true);});

    _actnCopyGraphData = Ori::Gui::action(tr("Copy Graph Data"), this, SLOT(copyGraphData2D()), ":/toolbar/copy");
    _plot->menuPlot->insertAction(actnCopyPlotImage, _actnCopyGraphData);
    _plot->menuAxisX->insertAction(actnCopyPlotImage, _actnCopyGraphData);
    _plot->menuAxisY->insertAction(actnCopyPlotImage, _actnCopyGraphData);

    menuLimits->addSeparator();
    menuLimits->addAction(_actnStabilityAutolimits);

    toolbar()->addSeparator();
    toolbar()->addAction(_actnStabilityAutolimits);
}

QWidget* StabilityMap2DWindow::makeOptionsPanel()
{
    return new StabilityMap2DOptionsPanel(this);
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
    auto f = function();
    auto rangeX = f->rangeX();
    auto rangeY = f->rangeY();
    int nx = rangeX.points();
    int ny = rangeY.points();
    auto resultsT = f->resultsT();
    auto resultsS = f->resultsS();
    auto results = actnShowS->isChecked() ? resultsS : resultsT;

    auto data = _graph->data();
    data->setSize(nx, ny);

    auto unitX = getUnitX();
    auto unitY = getUnitY();
    auto minX = unitX->fromSi(rangeX.start());
    auto maxX = unitX->fromSi(rangeX.stop());
    auto minY = unitY->fromSi(rangeY.start());
    auto maxY = unitY->fromSi(rangeY.stop());
    data->setRange({ minX, maxX }, { minY, maxY });

    _autolimiter->setData({minX, maxX}, {minY, maxY});

    for (int ix = 0; ix < nx; ix++)
        for (int iy = 0; iy < ny; iy++)
            data->setCell(ix, iy, results.at(ix * ny + iy));

    if (_zAutolimitsRequest)
    {
        autolimitsStability(false);
        _zAutolimitsRequest = false;
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

Z::Unit StabilityMap2DWindow::getDefaultUnitX() const
{
    return function()->paramX()->range.start.unit();
}

Z::Unit StabilityMap2DWindow::getDefaultUnitY() const
{
    return function()->paramY()->range.start.unit();
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

void StabilityMap2DWindow::storeViewSpecific(int key)
{
    ViewState view;
    view.limitsZ = _plot->limits(_colorScale->axis());
    _auxStoredView[key] = view;
}

void StabilityMap2DWindow::restoreViewSpecific(int key)
{
    if (_storedView.contains(key))
    {
        const ViewState& view = _auxStoredView[key];
        _plot->setLimits(_colorScale->axis(), view.limitsZ, false);
    }
    else
        _zAutolimitsRequest = true;
}

QString StabilityMap2DWindow::readFunction(const QJsonObject& root)
{
    function()->setStabilityCalcMode(Z::IO::Utils::enumFromStr(
        root["stab_calc_mode"].toString(), Z::Enums::StabilityCalcMode::Normal));
    auto resX = Z::IO::Json::readVariable(root["arg_x"].toObject(), function()->paramX(), schema());
    if (!resX.isEmpty()) return resX;
    auto resY = Z::IO::Json::readVariable(root["arg_y"].toObject(), function()->paramY(), schema());
    if (!resY.isEmpty()) return resY;
    return QString();
}

QString StabilityMap2DWindow::writeFunction(QJsonObject& root)
{
    root["stab_calc_mode"] = Z::IO::Utils::enumToStr(function()->stabilityCalcMode());
    root["arg_x"] = Z::IO::Json::writeVariable(function()->paramX(), schema());
    root["arg_y"] = Z::IO::Json::writeVariable(function()->paramY(), schema());
    return QString();
}

QString StabilityMap2DWindow::readWindowSpecific(const QJsonObject& root)
{
    // Restore plot limits
    QCPL::AxisLimits limitsZ { root["z_min"].toDouble(Double::nan()),
                               root["z_max"].toDouble(Double::nan()) };
    _zAutolimitsRequest = limitsZ.isInvalid();
    if (!_zAutolimitsRequest)
        _plot->setLimits(_colorScale->axis(), limitsZ, false);

    // Restore view states
    QJsonArray viewsJson = root["aux_stored_views"].toArray();
    for (auto it = viewsJson.begin(); it != viewsJson.end(); it++)
    {
        QJsonObject viewJson = (*it).toObject();
        auto mode = viewJson["mode"].toInt(-1);
        if (mode < 0) continue;
        ViewState state;
        state.limitsZ = { viewJson["z_min"].toDouble(Double::nan()),
                          viewJson["z_max"].toDouble(Double::nan()) };
        if (state.limitsZ.isInvalid())
            continue;
        _auxStoredView.insert(mode, state);
    }

    return QString();
}

QString StabilityMap2DWindow::writeWindowSpecific(QJsonObject& root)
{
    // Store plot limits
    auto limitsZ = _plot->limits(_colorScale->axis());
    root["z_min"] = limitsZ.min;
    root["z_max"] = limitsZ.max;

    // Store view states
    QJsonArray viewsJson;
    for (int key : _auxStoredView.keys())
    {
        auto view = _auxStoredView[key];
        viewsJson.append(QJsonObject{
            { "mode", key },
            { "z_min", view.limitsZ.min },
            { "z_max", view.limitsZ.max },
        });
    }
    root["aux_stored_views"] = viewsJson;

    return QString();
}

QString StabilityMap2DWindow::getCursorInfo(const QPointF& pos) const
{
    if (!function()->ok()) return QString();
    auto res = function()->calculateAt(Z::Value(pos.x(), getUnitX()), Z::Value(pos.y(), getUnitY()));
    return QStringLiteral("Pt = %1; Ps = %2").arg(Z::format(res.T)).arg(Z::format(res.S));
}

void StabilityMap2DWindow::copyGraphData2D()
{
    auto settings = PlotHelpers::makeExportSettings();
    bool transposed = settings.transposed;
    settings.transposed = false;
    auto exporter = QCPL::GraphDataExporter(settings);
    auto data =_graph->data();
    auto ny = data->valueSize();
    auto nx = data->keySize();
    qDebug() << "copy2d" << ny << nx;
    if (transposed)
    {
        QVector<double> v(ny);
        for (int ix = 0; ix < nx; ix++)
        {
            for (int iy = 0; iy < ny; iy++)
                v[iy] = data->cell(ix, iy);
            exporter.add(v);
        }
    }
    else
    {
        QVector<double> v(nx);
        for (int iy = 0; iy < ny; iy++)
        {
            for (int ix = 0; ix < nx; ix++)
                v[ix] = data->cell(ix, iy);
            exporter.add(v);
        }
    }
    exporter.toClipboard();
}
