# Steps to add a new plot function

See [code style rules](code-style.md) for naming convetion.

## Add function implementation class

- Function classes are under `src/funcs`.
- Derive the new class from `PlotFunction` base class.
- Describe the function using `FUNC_ALIAS` and `FUNC_NAME` macros and overriding `iconPath()` method. 

### BeamOverStabFunction.h
```cpp
#include "PlotFunction.h"

class BeamOverStabFunction : public PlotFunction
{
public:
    FUNC_ALIAS("BeamVsStab")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Over Stability"))
    
    BeamOverStabFunction(Schema *schema);
    
    const char* iconPath() const override {
        return ":/toolbar/func_beam_over_stab"; }
};
```

## Add new function window

- Function windows are under `src/funcs_window`.
- Window class name should be the same as the function class name but with `Window` suffix instead of `Function`.
- Derive the new class from `PlotFuncWindowStorable` base class.
- Create the function and pass it into the parent constructor.
- Function window should overload `function()` method and return a pointer to its specific function.

### BeamOverStabWindow.h
```cpp
#include "PlotFuncWindowStorable.h"
#include "../funcs/BeamOverStabFunction.h"

class BeamOverStabWindow : public PlotFuncWindowStorable
{
public:
    BeamOverStabWindow(Schema*);
    
    BeamOverStabFunction* function() const {
        return dynamic_cast<BeamOverStabFunction*>(_function); }
};
```

### BeamOverStabWindow.cpp
```cpp
#include "BeamOverStabWindow.h"

BeamOverStabWindow::BeamOverStabWindow(Schema *schema) 
    : PlotFuncWindowStorable(new BeamOverStabFunction(schema))
{
}
```

## Register function window constructor in `WindowsManager`

`WindowsManager` knows nothing about specific window types, but `CalcManager` does. So it should be done in `CalcManager` constructor. 

### CalcManager.cpp
```cpp
CalcManager::CalcManager(...)
{
	...
    registerWindowConstructor<BeamOverStabWindow, BeamOverStabFunction>();
}
```

## Make function caller

- All function is called via `CalcManager`'s slots.
- Functions should be called via `showPlotFunc()` method.

### CalcManager.h
```cpp
public slots:
    ...
    void funcBeamOverStab();
```

### CalcManager.cpp
```cpp
void CalcManager::funcBeamOverStab()
{
    showPlotFunc<BeamOverStabFunction>();
}
```

## Make action to call the function from `ProjectWindow`

- Action is created in `createActions()` method.
- Insert the action into the Functions menu in `createMenuBar()` method.
- Insert the action into the Functions toolbar in `createToolBars()` method.

### ProjectWindow.cpp
```cpp
void ProjectWindow::createActions()
{
    ...
    actnFuncBeamOverStab = A_(tr("&Beam Over Stability..."),
                              _calculations,
                              SLOT(funcBeamOverStab()),
                              ":/toolbar/func_beam_over_stab");
    ...
}
...
void ProjectWindow::createMenuBar()
{
    ...
    menuFunctions = Ori::Gui::menu(tr("F&unctions"), this, {
        ...
        actnFuncBeamOverStab,
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
        actnFuncBeamOverStab,
        ...
    }));
    ...
}
```

### Now the new empty function should be available

- The function will not be called if schema is empty. Add at least one element before calling the function.
- The function window can be saved in schema file already and will be restored. 
