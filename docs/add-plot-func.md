# Steps to add a new plot function

See [code style rules](code-style.md) for naming convetion.

## Basic precautions

### Add function implementation class

- Function classes are under `src/funcs`.
- Derive the new class from `PlotFunction` base class.
- Describe the function using `FUNC_ALIAS` and `FUNC_NAME` macros and overriding `iconPath()` method. 

#### CalcSomethingFunction.h
```cpp
#include "PlotFunction.h"

class CalcSomethingFunction : public PlotFunction
{
public:
    FUNC_ALIAS("CalcSomething")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Calculate Something"))
    
    CalcSomethingFunction(Schema *schema);
    
    const char* iconPath() const override { return ":/toolbar/func_calc_smth"; }
};
```

### Add new function window

- Function windows are under `src/funcs_window`.
- Window class name should be the same as the function class name but with `Window` suffix instead of `Function`.
- Derive the new class from `PlotFuncWindowStorable` base class.
- Create the function and pass it into the parent constructor.
- Function window should overload `function()` method and return a pointer to its specific function.

#### CalcSomethingWindow.h
```cpp
#include "PlotFuncWindowStorable.h"
#include "../funcs/CalcSomethingFunction.h"

class CalcSomethingWindow : public PlotFuncWindowStorable
{
public:
    CalcSomethingWindow(Schema*);
    
    CalcSomethingFunction* function() const {
        return dynamic_cast<CalcSomethingFunction*>(_function); }
};
```

#### CalcSomethingWindow.cpp
```cpp
#include "CalcSomethingWindow.h"

CalcSomethingWindow::CalcSomethingWindow(Schema *schema) 
    : PlotFuncWindowStorable(new CalcSomethingFunction(schema))
{
}
```

### Register function window constructor in `WindowsManager`

- `WindowsManager` should know nothing about specific window types, but `CalcManager` can. So it should be done in `CalcManager` constructor. 
- Use helper template function `registerWindowConstructor()`.

#### CalcManager.cpp
```cpp
#include "funcs_window/CalcSomethingWindow.h"

CalcManager::CalcManager(...)
{
	...
    registerWindowConstructor<CalcSomethingWindow, CalcSomethingFunction>();
}
```

### Make function caller

- All function is called via `CalcManager`'s slots.
- Functions should be called via `showPlotFunc()` method.

#### CalcManager.h
```cpp
public slots:
    ...
    void funcCalcSomething();
```

#### CalcManager.cpp
```cpp
void CalcManager::funcCalcSomething()
{
    showPlotFunc<CalcSomethingFunction>();
}
```

### Make action to call the function from `ProjectWindow`

- Action is created in `createActions()` method.
- Insert the action into the Functions menu in `createMenuBar()` method.
- Insert the action into the Functions toolbar in `createToolBars()` method.

#### ProjectWindow.cpp
```cpp
void ProjectWindow::createActions()
{
    ...
    actnFuncCalcSomething = A_(tr("&Calculate Something..."),
        _calculations, SLOT(funcCalcSomething()), ":/toolbar/func_calc_smth");
    ...
}
...
void ProjectWindow::createMenuBar()
{
    ...
    menuFunctions = Ori::Gui::menu(tr("F&unctions"), this, {
        ...
        actnFuncCalcSomething,
        ...
    });
    ...
}
...
void ProjectWindow::createToolBars()
{
    ...
    addToolBar(makeToolBar(tr("Functions"), {
        ...
        actnFuncCalcSomething,
        ...
    }));
    ...
}
```

#### Now the new empty function should be available

- The function will not be called if schema is empty. Add at least one element before calling the function.
- The function window can be saved in schema file already and will be restored. 
