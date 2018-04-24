#include "CalcManager.h"
#include "WindowsManager.h"
#include "core/Protocol.h"
#include "funcs/InfoFunctions.h"
#include "funcs_window/InfoFuncWindow.h"
#include "funcs_window/CausticWindow.h"
#include "funcs_window/StabilityMapWindow.h"
#include "funcs_window/StabilityMap2DWindow.h"

template <class TWindow> SchemaWindow* windowConstructor(Schema* schema)
{
    return new TWindow(schema);
}

template <class TWindow, class TFunction> void registerWindowConstructor()
{
    WindowsManager::registerConstructor(TFunction::_alias_(), windowConstructor<TWindow>);
}

#define RETURN_IF_SCHEMA_EMPTY \
    if (schema()->isEmpty()) \
    { \
        Z_INFO("Schema is empty"); \
        return; \
    }

CalcManager::CalcManager(Schema *schema, QWidget *parent) :
    QObject(parent), _parent(parent), _schema(schema)
{
    registerWindowConstructor<StabilityMapWindow, StabilityMapFunction>();
    registerWindowConstructor<StabilityMap2DWindow, StabilityMap2DFunction>();
    registerWindowConstructor<CausticWindow, CausticFunction>();
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
    showPlotFunc<StabilityMapWindow>();
}

void CalcManager::funcStabMap2d()
{
    showPlotFunc<StabilityMap2DWindow>();
}

void CalcManager::funcRepRate()
{
    RETURN_IF_SCHEMA_EMPTY
    showInfoFunc(new InfoFuncRepetitionRate(schema()));
}

void CalcManager::funcCaustic()
{
    showPlotFunc<CausticWindow>();
}

void CalcManager::funcShowMatrices()
{
    RETURN_IF_SCHEMA_EMPTY
    auto elems = schema()->selectedElements();
    if (elems.isEmpty())
    {
        Z_INFO("No elements selected");
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

template <class TWindow> void CalcManager::showPlotFunc()
{
    RETURN_IF_SCHEMA_EMPTY
    auto wnd = new TWindow(schema());
    if (wnd->configure(_parent))
    {
        WindowsManager::instance().show(wnd);
        wnd->requestAutolimits();
        wnd->requestCenterCursor();
        wnd->update();
    }
}
