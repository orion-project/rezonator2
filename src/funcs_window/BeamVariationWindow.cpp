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

BeamVariationParamsDlg::BeamVariationParamsDlg(Schema *schema, Z::Variable *var)
    : RezonatorDialog(DontDeleteOnClose), _var(var)
{
    setWindowTitle(tr("Variable"));
    setObjectName("StabilityParamsDlg");

    if (!var->element && !_recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(_recentKey), var, schema);

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

}

void BeamVariationParamsDlg::collect()
{

}

void BeamVariationParamsDlg::guessRange()
{

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
    return BeamVariationParamsDlg(schema(), function()->arg()).run();
}
