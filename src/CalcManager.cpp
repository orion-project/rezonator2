#include "CalcManager.h"
#include "WindowsManager.h"
#include "core/Protocol.h"
#include "funcs/BeamVariationFunction.h"
#include "funcs/CausticFunction.h"
#include "funcs/InfoFunctions.h"
#include "funcs/BeamParamsAtElemsFunction.h"
#include "funcs/MultibeamCausticFunction.h"
#include "funcs/MultirangeCausticFunction.h"
#include "funcs/StabilityMapFunction.h"
#include "funcs/StabilityMap2DFunction.h"
#include "funcs_window/BeamVariationWindow.h"
#include "funcs_window/CausticWindow.h"
#include "funcs_window/InfoFuncWindow.h"
#include "funcs_window/PlotFuncWindow.h"
#include "funcs_window/MultibeamCausticWindow.h"
#include "funcs_window/MultirangeCausticWindow.h"
#include "funcs_window/StabilityMapWindow.h"
#include "funcs_window/StabilityMap2DWindow.h"
#include "funcs_window/TableFuncWindow.h"

template <class TWindow> SchemaWindow* windowConstructor(Schema* schema)
{
    return new TWindow(schema);
}

template <class TWindow, class TFunction> void registerWindowConstructor()
{
    WindowsManager::registerConstructor(TFunction::_alias_(), windowConstructor<TWindow>);
}

#define RETURN_IF_SCHEMA_EMPTY \
    if (schema()->activeCount() == 0) \
    { \
        Z_INFO("There are no active elements in the schema"); \
        return; \
    }

CalcManager::CalcManager(Schema *schema, QWidget *parent) :
    QObject(parent), _parent(parent), _schema(schema)
{
    registerWindowConstructor<StabilityMapWindow, StabilityMapFunction>();
    registerWindowConstructor<StabilityMap2DWindow, StabilityMap2DFunction>();
    registerWindowConstructor<CausticWindow, CausticFunction>();
    registerWindowConstructor<MultirangeCausticWindow, MultirangeCausticFunction>();
    registerWindowConstructor<MultibeamCausticWindow, MultibeamCausticFunction>();
    registerWindowConstructor<BeamVariationWindow, BeamVariationFunction>();
}

void CalcManager::funcSummary()
{
    showInfoFunc(new InfoFuncSummary(schema()));
}

void CalcManager::funcRoundTrip()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncMatrixRT(schema(), schema()->selectedElement()));
}

void CalcManager::funcMultFwd()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncMatrixMultFwd(schema(), schema()->selectedElements()));
}

void CalcManager::funcMultBkwd()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncMatrixMultBkwd(schema(), schema()->selectedElements()));
}

void CalcManager::funcStabMap()
{
    showPlotFunc<StabilityMapFunction>();
}

void CalcManager::funcStabMap2d()
{
    showPlotFunc<StabilityMap2DFunction>();
}

void CalcManager::funcRepRate()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncRepetitionRate(schema()));
}

void CalcManager::funcCaustic()
{
    showPlotFunc<CausticFunction>();
}

void CalcManager::funcMultirangeCaustic()
{
    showPlotFunc<MultirangeCausticFunction>();
}

void CalcManager::funcMultibeamCaustic()
{
    showPlotFunc<MultibeamCausticFunction>();
}

void CalcManager::funcBeamVariation()
{
    showPlotFunc<BeamVariationFunction>();
}

void CalcManager::funcBeamParamsAtElems()
{
    showTableFunc<BeamParamsAtElemsFunction>();
}

void CalcManager::funcShowMatrices()
{
    RETURN_IF_SCHEMA_EMPTY
    auto elems = schema()->selectedElements();
    if (elems.isEmpty())
    {
        Z_INFO("No elements selected")
        return;
    }
    if (elems.size() == 1)
        showInfoFunc(new InfoFuncMatrix(schema(), elems.at(0)));
    else
        showInfoFunc(new InfoFuncMatrices(schema(), elems));
}

void CalcManager::funcShowAllMatrices()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncMatrices(schema(), schema()->elements()));
}

void CalcManager::showInfoFunc(InfoFunction* func)
{
    InfoFuncWindow::open(func, _parent);
}

template <class TFunction> void CalcManager::showPlotFunc()
{
    RETURN_IF_SCHEMA_EMPTY

    auto ctor = WindowsManager::getConstructor(TFunction::_alias_());
    if (!ctor) return;

    auto wnd = ctor(schema());
    if (!wnd) return;

    auto plotWnd = dynamic_cast<PlotFuncWindow*>(wnd);
    if (!plotWnd or !plotWnd->configure())
    {
        delete wnd;
        return;
    }

    plotWnd->function()->loadPrefs();
    WindowsManager::instance().show(wnd);
    plotWnd->requestAutolimits();
    plotWnd->requestCenterCursor();
    plotWnd->update();
}

template <class TFunction> void CalcManager::showTableFunc()
{
    RETURN_IF_SCHEMA_EMPTY

    auto wnd = new TableFuncWindow(new TFunction(schema()));

    WindowsManager::instance().show(wnd);
    wnd->update();
}
