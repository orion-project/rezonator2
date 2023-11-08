#include "CausticWindow.h"

#include "../app/AppSettings.h"
#include "../app/CustomPrefs.h"
#include "../funcs/BeamShapeExtension.h"
#include "../funcs/CausticOptionsPanel.h"
#include "../io/CommonUtils.h"
#include "../io/JsonUtils.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriWidgets.h"

#include <QAction>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QToolBar>

#include <qcpl_plot.h>

//------------------------------------------------------------------------------
//                           CausticParamsDlg
//------------------------------------------------------------------------------

CausticParamsDlg::CausticParamsDlg(Schema *schema, Z::Variable *var)
    : RezonatorDialog(DontDeleteOnClose), _var(var)
{
    setWindowTitle(tr("Range"));
    setObjectName("CausticParamsDlg");

    if (!var->element and !_recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(_recentKey), var, schema);

    std::shared_ptr<ElementFilter> filter(
        ElementFilter::make<ElementFilterIsRange, ElementFilterEnabled>());

    _elemSelector = new ElemSelectorWidget(schema, filter.get());
    connect(_elemSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(guessRange()));

    _rangeEditor = new PointsRangeEditor;

    auto layoutElement = new QHBoxLayout;
    layoutElement->addWidget(new QLabel(tr("Element")));
    layoutElement->addSpacing(8);
    layoutElement->addWidget(_elemSelector);
    layoutElement->setStretch(2, 1);

    mainLayout()->addLayout(layoutElement);
    mainLayout()->addSpacing(8);
    mainLayout()->addWidget(Ori::Gui::group(tr("Plot accuracy"), _rangeEditor));
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();

    populate();
}

void CausticParamsDlg::populate()
{
    if (_var->element) // edit variable
    {
        _elemSelector->setSelectedElement(_var->element);
        _rangeEditor->setRange(_var->range);
    }
    else // 'create' variable
    {
        // TODO guess or restore from settings
        guessRange();
    }
}

void CausticParamsDlg::collect()
{
    auto res = _elemSelector->verify();
    if (!res) return res.show(this);

    res = _rangeEditor->verify();
    if (!res) return res.show(this);

    _var->element = _elemSelector->selectedElement();
    _var->parameter = Z::Utils::asRange(_var->element)->paramLength();
    _var->range = _rangeEditor->range();
    accept();

    if (!_recentKey.isEmpty())
        CustomPrefs::setRecentObj(_recentKey, Z::IO::Json::writeVariablePref(_var));
}

void CausticParamsDlg::guessRange()
{
    auto elem = _elemSelector->selectedElement();
    if (!elem) return;

    auto elemRange = Z::Utils::asRange(elem);
    if (!elemRange) return;

    // TODO restore or guess step
    Z::VariableRange range;
    range.stop = Z::Utils::getRangeStop(elemRange);
    range.start = range.stop * 0.0;
    range.step = range.stop / 100.0;
    _rangeEditor->setRange(range);
}

//------------------------------------------------------------------------------
//                                CausticWindow
//------------------------------------------------------------------------------

CausticWindow::CausticWindow(Schema *schema) : PlotFuncWindowStorable(new CausticFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);

    addTextVar("{func_mode}", tr("Function mode"), [this]{ return CausticFunction::modeDisplayName(function()->mode()); });

    _plot->addTextVarX("{elem}", tr("Element label and title"), [this]{
        return function()->arg()->element->displayLabelTitle(); });
    _plot->addTextVarX("{elem_label}", tr("Element label"), [this]{
        return function()->arg()->element->label(); });
    _plot->addTextVarX("{elem_title}", tr("Element title"), [this]{
        return function()->arg()->element->title(); });

    _plot->setDefaultTextX("{elem} {(unit)}");
    _plot->setFormatterTextX(_plot->defaultTextX());
    _plot->setDefaultTextY("{func_mode} {(unit)}");
    _plot->setFormatterTextY(_plot->defaultTextY());
}

QList<BasicMdiChild::MenuItem> CausticWindow::viewMenuItems() const
{
    return { _beamShape->actionToggle() };
}

bool CausticWindow::configureInternal()
{
    return CausticParamsDlg(schema(), function()->arg()).run();
}

QWidget* CausticWindow::makeOptionsPanel()
{
    return new CausticOptionsPanel<CausticWindow>(this);
}

QString CausticWindow::readFunction(const QJsonObject& root)
{
    function()->setMode(Z::IO::Utils::enumFromStr(
        root["mode"].toString(), CausticFunction::BeamRadius));
    auto res = Z::IO::Json::readVariable(root["arg"].toObject(), function()->arg(), schema());
    if (!res.isEmpty())
        return res;
    return QString();
}

QString CausticWindow::writeFunction(QJsonObject& root)
{
    root["mode"] = Z::IO::Utils::enumToStr(function()->mode());
    root["arg"] = Z::IO::Json::writeVariable(function()->arg(), schema());
    return QString();
}

void CausticWindow::storeView(FuncMode mode)
{
    ViewSettings vs;
    storeViewParts(vs, VP_LIMITS_Y | VP_TITLE_Y | VP_UNIT_Y | VP_CUSRSOR_POS);
    _storedView[mode] = vs;
}

void CausticWindow::restoreView(FuncMode mode)
{
    ViewSettings vs;
    if (_storedView.contains(mode))
        vs = _storedView[mode];
    restoreViewParts(vs, VP_LIMITS_Y | VP_TITLE_Y | VP_UNIT_Y | VP_CUSRSOR_POS);
}

Z::Unit CausticWindow::getDefaultUnitX() const
{
    return function()->arg()->parameter->value().unit();
}

Z::Unit CausticWindow::getDefaultUnitY() const
{
    return getDefaultUnitY(function()->mode());
}

Z::Unit CausticWindow::getDefaultUnitY(FuncMode mode) const
{
    switch (mode)
    {
    case CausticFunction::BeamRadius: return AppSettings::instance().defaultUnitBeamRadius;
    case CausticFunction::FrontRadius: return AppSettings::instance().defaultUnitFrontRadius;
    case CausticFunction::HalfAngle: return AppSettings::instance().defaultUnitAngle;
    }
    return Z::Units::none();
}

SpecPointParams CausticWindow::getSpecPointsParams() const
{
    return {
        { CausticFunction::spUnitX, SpecPointParam(getUnitX()) },
        { CausticFunction::spUnitW, SpecPointParam(function()->mode() == CausticFunction::BeamRadius ? getUnitY() : getUnitY(CausticFunction::BeamRadius)) },
        { CausticFunction::spUnitR, SpecPointParam(function()->mode() == CausticFunction::FrontRadius ? getUnitY() : getUnitY(CausticFunction::FrontRadius)) },
    };
}

void CausticWindow::getCursorInfo(const Z::ValuePoint& pos, CursorInfoValues& values) const
{
    if (!function()->ok()) return;
    auto res = function()->calculateAt(pos.X);
    _beamShape->setShape(res);
    QString valueName = CausticFunction::modeAlias(function()->mode());
    values << CursorInfoValue(valueName+'t', res.T);
    values << CursorInfoValue(valueName+'s', res.S);
}
