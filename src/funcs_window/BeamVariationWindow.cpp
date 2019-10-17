#include "BeamVariationWindow.h"

#include "../CustomPrefs.h"
#include "../io/JsonUtils.h"
#include "../widgets/Plot.h"
#include "../widgets/ElemSelectorWidget.h"
#include "../widgets/VariableRangeEditor.h"

#include "helpers/OriWidgets.h"

//------------------------------------------------------------------------------
//                             BeamVariationParamsDlg
//------------------------------------------------------------------------------

BeamVariationParamsDlg::BeamVariationParamsDlg(Schema *schema, Z::Variable *var, Z::PlotPosition *pos)
    : RezonatorDialog(DontDeleteOnClose), _schema(schema), _var(var), _pos(pos)
{
    setWindowTitle(tr("Variable"));
    setObjectName("BeamVariationParamsDlg");

    std::shared_ptr<ElementFilter> elemFilter(
        ElementFilter::make<ElementFilterHasVisibleParams, ElementFilterEnabled>());

    _elemSelector = new ElemAndParamSelector(schema, elemFilter.get(), Z::Utils::defaultParamFilter());
    connect(_elemSelector, SIGNAL(selectionChanged()), this, SLOT(guessRange()));

    _placeSelector = new ElemOffsetSelectorWidget(schema, nullptr);

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

}

bool BeamVariationWindow::configureInternal()
{
    return BeamVariationParamsDlg(schema(), function()->arg(), function()->pos()).run();
}

QString BeamVariationWindow::getDefaultTitle() const
{
    return tr("Beam Radius Variation");
}

QString BeamVariationWindow::getDefaultTitleX() const
{
    auto arg = function()->arg();
    auto unit = getUnitX();
    if (unit == Z::Units::none())
        return QStringLiteral("%1, %2")
                .arg(arg->element->displayLabelTitle())
                .arg(arg->parameter->name());
    return QStringLiteral("%1, %2 (%3)")
            .arg(arg->element->displayLabelTitle())
            .arg(arg->parameter->label())
            .arg(unit->name());
}

QString BeamVariationWindow::getDefaultTitleY() const
{
    auto elem = function()->pos()->element;
    auto range = Z::Utils::asRange(elem);
    if (range)
        return tr("Beam radius at %1 +%2, (%3)")
            .arg(elem->label())
            .arg(function()->pos()->offset.displayStr())
            .arg(getUnitY()->name());
    return tr("Beam radius at %1, (%2)")
        .arg(elem->label())
        .arg(getUnitY()->name());
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
