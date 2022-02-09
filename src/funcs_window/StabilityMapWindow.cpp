#include "StabilityMapWindow.h"

#include "FuncOptionsPanel.h"
#include "../CustomPrefs.h"
#include "../core/Format.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include "qcpl_plot.h"

#include <QAction>

//------------------------------------------------------------------------------
//                              StabilityParamsDlg
//------------------------------------------------------------------------------

StabilityParamsDlg::StabilityParamsDlg(Schema *schema, Z::Variable *var)
    : RezonatorDialog(DontDeleteOnClose), _var(var)
{
    setWindowTitle(tr("Variable"));
    setObjectName("StabilityParamsDlg");

    if (!var->element && !_recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(_recentKey), var, schema);

    _elemSelector = new ElemAndParamSelector(schema, ElementFilter::elemsWithVisibleParams(), Z::Utils::defaultParamFilter());
    connect(_elemSelector, SIGNAL(selectionChanged()), this, SLOT(guessRange()));

    _rangeEditor = new GeneralRangeEditor;

    mainLayout()->addLayout(_elemSelector);
    mainLayout()->addSpacing(8);
    mainLayout()->addWidget(Ori::Gui::group(tr("Variation"), _rangeEditor));
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();

    populate();
}

void StabilityParamsDlg::populate()
{
    if (_var->element) // edit variable
    {
        _elemSelector->setSelectedElement(_var->element);
        _elemSelector->setSelectedParameter(_var->parameter);
        _rangeEditor->setRange(_var->range);
    }
    else // 'create' variable
    {
        // TODO guess or restore from settings
        guessRange();
    }
}

void StabilityParamsDlg::collect()
{
    auto res = _elemSelector->verify();
    if (!res) return res.show(this);

    res = _rangeEditor->verify();
    if (!res) return res.show(this);

    _var->element = _elemSelector->selectedElement();
    _var->parameter = _elemSelector->selectedParameter();
    _var->range = _rangeEditor->range();

    accept();

    if (!_recentKey.isEmpty())
        CustomPrefs::setRecentObj(_recentKey, Z::IO::Json::writeVariablePref(_var));
}

void StabilityParamsDlg::guessRange()
{
    auto param = _elemSelector->selectedParameter();
    if (!param) return;

    // TODO restore or guess range limits and step
    Z::VariableRange range;
    range.start = param->value();
    range.stop = param->value();
    range.step = param->value() * 0;
    _rangeEditor->setRange(range);
}

//------------------------------------------------------------------------------
//                            StabilityMapOptionsPanel
//------------------------------------------------------------------------------

class StabilityMapOptionsPanel : public FuncOptionsPanel
{
public:
    StabilityMapOptionsPanel(StabilityMapWindow* window);
    int currentFunctionMode() const override;
    void functionModeChanged(int mode) override;

private:
    StabilityMapWindow* _window;
};

StabilityMapOptionsPanel::StabilityMapOptionsPanel(StabilityMapWindow* window) : FuncOptionsPanel(window), _window(window)
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

int StabilityMapOptionsPanel::currentFunctionMode() const
{
    return static_cast<int>(_window->function()->stabilityCalcMode());
}

void StabilityMapOptionsPanel::functionModeChanged(int mode)
{
    auto stabCalcMode = static_cast<Z::Enums::StabilityCalcMode>(mode);
    CustomPrefs::setRecentStr(QStringLiteral("func_stab_map_mode"), Z::Enums::toStr(stabCalcMode));
    _window->function()->setStabilityCalcMode(stabCalcMode);
}

//------------------------------------------------------------------------------
//                             StabilityMapWindow
//------------------------------------------------------------------------------

StabilityMapWindow::StabilityMapWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMapFunction(schema))
{
    auto getStabParam = [this]{ return Z::Enums::displayStr(function()->stabilityCalcMode()); };
    _plot->addTextVar(QStringLiteral("{stab_mode}"), tr("Stability parameter mode"), getStabParam);

    _plot->addTextVarX(QStringLiteral("{elem}"), tr("Variable element label and title"), [this]{
        return function()->arg()->element->displayLabelTitle(); });
    _plot->addTextVarX(QStringLiteral("{elem_label}"), tr("Variable element label"), [this]{
        return function()->arg()->element->label(); });
    _plot->addTextVarX(QStringLiteral("{elem_title}"), tr("Variable element title"), [this]{
        return function()->arg()->element->title(); });
    _plot->addTextVarX(QStringLiteral("{elem_param}"), tr("Variable element parameter"), [this]{
        return function()->arg()->parameter->name(); });

    _plot->addTextVarY(QStringLiteral("{stab_mode}"), tr("Stability parameter mode"), getStabParam);

    _plot->setDefaultTitleX(QStringLiteral("{elem}, {elem_param} {(unit)}"));
    _plot->setFormatterTextX(QStringLiteral("{elem}, {elem_param} {(unit)}"));
    _plot->setDefaultTitleY(QStringLiteral("Stability parameter {stab_mode}"));
    _plot->setFormatterTextY(QStringLiteral("Stability parameter {stab_mode}"));

    _actnStabilityAutolimits = new QAction(tr("Y-axis -> Stability Range", "Plot action"), this);
    _actnStabilityAutolimits->setIcon(QIcon(":/toolbar/limits_stab"));
    connect(_actnStabilityAutolimits, &QAction::triggered, this, &StabilityMapWindow::autolimitsStability);

    _actnStabBoundMarkers = new QAction(tr("Stability boundary markers"), this);
    _actnStabBoundMarkers->setCheckable(true);
    _actnStabBoundMarkers->setChecked(true);
    connect(_actnStabBoundMarkers, &QAction::toggled, this, &StabilityMapWindow::toggleStabBoundMarkers);

    menuLimits->addSeparator();
    menuLimits->addAction(_actnStabilityAutolimits);

    toolbar()->addSeparator();
    toolbar()->addAction(_actnStabilityAutolimits);

    _stabBoundMarkerLow = makeStabBoundMarker();
    _stabBoundMarkerTop = makeStabBoundMarker();
}

bool StabilityMapWindow::configureInternal()
{
    return StabilityParamsDlg(schema(), function()->arg()).run();
}

void StabilityMapWindow::autolimitsStability()
{
    switch (function()->stabilityCalcMode())
    {
    case Z::Enums::StabilityCalcMode::Normal:
        _plot->setLimitsY(-1.05, 1.05);
        break;

    case Z::Enums::StabilityCalcMode::Squared:
        _plot->setLimitsY(-0.05, 1.05);
        break;
    }
}

QWidget* StabilityMapWindow::makeOptionsPanel()
{
    return new StabilityMapOptionsPanel(this);
}

QString StabilityMapWindow::readFunction(const QJsonObject& root)
{
    function()->setStabilityCalcMode(Z::IO::Utils::enumFromStr(
        root["stab_calc_mode"].toString(), Z::Enums::StabilityCalcMode::Normal));
    auto res = Z::IO::Json::readVariable(root["arg"].toObject(), function()->arg(), schema());
    if (!res.isEmpty())
        return res;
    return QString();
}

QString StabilityMapWindow::writeFunction(QJsonObject& root)
{
    root["stab_calc_mode"] = Z::IO::Utils::enumToStr(function()->stabilityCalcMode());
    root["arg"] = Z::IO::Json::writeVariable(function()->arg(), schema());
    return QString();
}

QString StabilityMapWindow::readWindowSpecific(const QJsonObject& root)
{
    _actnStabBoundMarkers->setChecked(root["stab_bound_markers"].toBool(true));
    return QString();
}

QString StabilityMapWindow::writeWindowSpecific(QJsonObject& root)
{
    root["stab_bound_markers"] = _actnStabBoundMarkers->isChecked();
    return QString();
}

void StabilityMapWindow::afterUpdate()
{
    updateStabBoundMarkers();
}

void StabilityMapWindow::updateStabBoundMarkers()
{
    _stabBoundMarkerTop->point1->setCoords(0, 1);
    _stabBoundMarkerTop->point2->setCoords(1, 1);
    _stabBoundMarkerTop->setVisible(_actnStabBoundMarkers->isChecked());

    double low = function()->stabilityCalcMode() ==
        (Z::Enums::StabilityCalcMode::Normal or actnShowFlippedTS->isChecked()) ? -1 : 0;
    _stabBoundMarkerLow->point1->setCoords(0, low);
    _stabBoundMarkerLow->point2->setCoords(1, low);
    _stabBoundMarkerLow->setVisible(_actnStabBoundMarkers->isChecked());
}

QCPItemStraightLine* StabilityMapWindow::makeStabBoundMarker() const
{
    QCPItemStraightLine *line = new QCPItemStraightLine(plot());
    line->setPen(QPen(Qt::magenta, 1, Qt::DashLine)); // TODO make configurable
    line->setSelectable(false);
    return line;
}

void StabilityMapWindow::fillViewMenuActions(QList<QAction*>& actions) const
{
    actions << _actnStabBoundMarkers;
}

void StabilityMapWindow::toggleStabBoundMarkers(bool on)
{
    _stabBoundMarkerLow->setVisible(on);
    _stabBoundMarkerTop->setVisible(on);
    plot()->replot();
    schema()->events().raise(SchemaEvents::Changed, "StabilityMapWindow: toggleStabBoundMarkers");
}

Z::Unit StabilityMapWindow::getDefaultUnitX() const
{
    return function()->arg()->range.start.unit();
}

QString StabilityMapWindow::getCursorInfo(const QPointF& pos) const
{
    if (!function()->ok()) return QString();
    auto res = function()->calculateAt(Z::Value(pos.x(), getUnitX()));
    return QStringLiteral("Pt = %1; Ps = %2").arg(Z::format(res.T), Z::format(res.S));
}
