#include "CalcManager.h"
#include "WindowsManager.h"
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
    if (!schema()->isEmpty())
        showInfoFunc(new InfoFuncMatrixRT(schema(), schema()->selectedElement()));
}

void CalcManager::funcMultFwd()
{
    if (!schema()->isEmpty())
        showInfoFunc(new InfoFuncMatrixMultFwd(schema(), schema()->selectedElements()));
}

void CalcManager::funcMultBkwd()
{
    if (!schema()->isEmpty())
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
    if (!schema()->isEmpty())
        showInfoFunc(new InfoFuncRepetitionRate(schema()));
}

void CalcManager::funcCaustic()
{
    showPlotFunc<CausticWindow>();
}

void CalcManager::showInfoFunc(InfoFunction* func)
{
    InfoFuncWindow::open(func, _parent);
}

template <class TWindow> void CalcManager::showPlotFunc()
{
    if (schema()->isEmpty()) return;

    auto wnd = new TWindow(schema());
    if (wnd->configure(_parent))
    {
        WindowsManager::instance().show(wnd);
        wnd->update();
    }
}
