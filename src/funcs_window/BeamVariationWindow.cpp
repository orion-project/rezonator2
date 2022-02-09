#include "BeamVariationWindow.h"

#include "BeamShapeExtension.h"
#include "../CustomPrefs.h"
#include "../core/Format.h"
#include "../io/JsonUtils.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriWidgets.h"

#include "qcpl_plot.h"

//------------------------------------------------------------------------------
//                             BeamVariationParamsDlg
//------------------------------------------------------------------------------

BeamVariationParamsDlg::BeamVariationParamsDlg(Schema *schema, Z::Variable *var, Z::PlotPosition *pos)
    : RezonatorDialog(DontDeleteOnClose), _schema(schema), _var(var), _pos(pos)
{
    setWindowTitle(tr("Variable"));
    setObjectName("BeamVariationParamsDlg");

    _elemSelector = new ElemAndParamSelector(schema, ElementFilter::elemsWithVisibleParams(), Z::Utils::defaultParamFilter());
    connect(_elemSelector, SIGNAL(selectionChanged()), this, SLOT(guessRange()));

    _placeSelector = new ElemOffsetSelectorWidget(schema, ElementFilter::enabledElements());

    _rangeEditor = new GeneralRangeEditor;

    mainLayout()->addLayout(_elemSelector);
    mainLayout()->addSpacing(8);
    mainLayout()->addWidget(Ori::Gui::group(tr("Variation"), _rangeEditor));
    mainLayout()->addSpacing(8);
    mainLayout()->addWidget(Ori::Gui::group(tr("Plot position"), _placeSelector));
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();

    populate();
}

void BeamVariationParamsDlg::populate()
{
    if (!_var->element) // create variable
    {
        QJsonObject pref = CustomPrefs::recentObj(_recentKey);

        Z::IO::Json::readVariablePref(pref, _var, _schema);

        _pos->element = _schema->elementByLabel(pref["plot_position_label"].toString());
        auto offset = Z::IO::Json::readValue(pref["offset"].toObject(), Z::Dims::linear());
        _pos->offset = offset.ok() ? offset.value() : 0_mm;
    }

    _elemSelector->setSelectedElement(_var->element);
    _elemSelector->setSelectedParameter(_var->parameter);
    _rangeEditor->setRange(_var->range);
    _placeSelector->setSelectedElement(_pos->element);
    _placeSelector->setOffset(_pos->offset);
}

void BeamVariationParamsDlg::collect()
{
    auto res = _elemSelector->verify();
    if (!res) return res.show(this);

    res = _rangeEditor->verify();
    if (!res) return res.show(this);

    res = _placeSelector->verify();
    if (!res) return res.show(this);

    _var->element = _elemSelector->selectedElement();
    _var->parameter = _elemSelector->selectedParameter();
    _var->range = _rangeEditor->range();

    _pos->element = _placeSelector->selectedElement();
    _pos->offset = _placeSelector->offset();

    accept();

    QJsonObject pref = Z::IO::Json::writeVariablePref(_var);
    pref["plot_position_label"] = _pos->element->label();
    pref["offset"] = Z::IO::Json::writeValue(_pos->offset);
    CustomPrefs::setRecentObj(_recentKey, pref);
}

void BeamVariationParamsDlg::guessRange()
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
//                              BeamVariationWindow
//------------------------------------------------------------------------------

BeamVariationWindow::BeamVariationWindow(Schema *schema)
    : PlotFuncWindowStorable(new BeamVariationFunction(schema))
{
    _beamShape = new BeamShapeExtension(this);

    _plot->addTextVarX(QStringLiteral("{elem}"), tr("Variable element label and title"), [this]{
        return function()->arg()->element->displayLabelTitle(); });
    _plot->addTextVarX(QStringLiteral("{elem_label}"), tr("Variable element label"), [this]{
        return function()->arg()->element->label(); });
    _plot->addTextVarX(QStringLiteral("{elem_title}"), tr("Variable element title"), [this]{
        return function()->arg()->element->title(); });
    _plot->addTextVarX(QStringLiteral("{elem_param}"), tr("Variable element parameter"), [this]{
        return function()->arg()->parameter->name(); });

    _plot->addTextVarY(QStringLiteral("{place}"), tr("Target element (with offset)"), [this]{
        auto pos = function()->pos();
        if (Z::Utils::isRange(pos->element))
            return QStringLiteral("%1 +%2").arg(pos->element->label(), pos->offset.displayStr());
        return pos->element->label();
    });

    _plot->setDefaultTitleX(QStringLiteral("{elem}, {elem_param} {(unit)}"));
    _plot->setFormatterTextX(QStringLiteral("{elem}, {elem_param} {(unit)}"));
    _plot->setDefaultTitleY(QStringLiteral("Beam radius at {place} {(unit)}"));
    _plot->setFormatterTextY(QStringLiteral("Beam radius at {place} {(unit)}"));
}

bool BeamVariationWindow::configureInternal()
{
    return BeamVariationParamsDlg(schema(), function()->arg(), function()->pos()).run();
}

Z::Unit BeamVariationWindow::getDefaultUnitX() const
{
    return function()->arg()->range.start.unit();
}

Z::Unit BeamVariationWindow::getDefaultUnitY() const
{
    return AppSettings::instance().defaultUnitBeamRadius;
}

QString BeamVariationWindow::readFunction(const QJsonObject& root)
{
    auto res = Z::IO::Json::readVariable(root["arg"].toObject(), function()->arg(), schema());
    if (!res.isEmpty())
        return res;

    auto pos = root["pos"].toObject();

    auto elem = Z::IO::Json::readElemByIndex(pos, "element_index", schema());
    if (!elem.ok()) return elem.error();

    auto offset = Z::IO::Json::readValue(pos["offset"].toObject());
    if (!offset.ok()) return offset.error();

    function()->pos()->element = elem.value();
    function()->pos()->offset = offset.value();
    return QString();
}

QString BeamVariationWindow::writeFunction(QJsonObject& root)
{
    root["arg"] = Z::IO::Json::writeVariable(function()->arg(), schema());
    root["pos"] = QJsonObject({
        { "element_index", schema()->indexOf(function()->pos()->element) },
        { "offset", Z::IO::Json::writeValue(function()->pos()->offset) },
    });
    return QString();
}

QString BeamVariationWindow::getCursorInfo(const QPointF& pos) const
{
    if (!function()->ok()) return QString();
    auto res = function()->calculateAt(Z::Value(pos.x(), getUnitX()));
    _beamShape->setShape(res);
    auto unitY = getUnitY();
    QString resT = Double(res.T).isNan() ? QStringLiteral("NaN") : Z::format(unitY->fromSi(res.T));
    QString resS = Double(res.S).isNan() ? QStringLiteral("NaN") : Z::format(unitY->fromSi(res.S));
    return QStringLiteral("Wt = %1; Ws = %2").arg(resT, resS);
}
