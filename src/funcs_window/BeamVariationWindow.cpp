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

    _placeSelector = new ElemOffsetSelectorWidget(schema, elemFilter.get());

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
