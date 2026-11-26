# 添加新的绘图函数的步骤

命名约定见[代码风格规则](code-style.md)。

## 基本注意事项

### 添加函数实现类

- 函数类位于 `src/math` 下。
- 新类应从 `PlotFunction` 基类派生。
- 使用 `FUNC_ALIAS` 和 `FUNC_NAME` 宏并重写 `iconPath()` 方法来描述该函数。

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

### 添加新的函数窗口

- 函数窗口位于 `src/funcs` 下。
- 窗口类名应与函数类名相同，但后缀是 `Window` 而不是 `Function`。
- 新类应从 `PlotFuncWindowStorable` 基类派生。
- 创建该函数，并将其传入父类构造函数。
- 函数窗口应重载 `function()` 方法，并返回指向其具体函数的指针。

#### CalcSomethingWindow.h
```cpp
#include "PlotFuncWindowStorable.h"
#include "../math/CalcSomethingFunction.h"

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

### 在 `WindowsManager` 中注册函数窗口构造函数

- `WindowsManager` 不应了解具体的窗口类型，但 `CalcManager` 可以。因此这应在 `CalcManager` 构造函数中完成。
- 使用辅助模板函数 `registerWindowConstructor()`。

#### CalcManager.cpp
```cpp
#include "funcs/CalcSomethingWindow.h"

CalcManager::CalcManager(...)
{
	...
    registerWindowConstructor<CalcSomethingWindow, CalcSomethingFunction>();
}
```

### 编写函数调用入口

- 所有函数都通过 `CalcManager` 的槽（slots）调用。
- 函数应通过 `showPlotFunc()` 方法调用。

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

### 添加从 `ProjectWindow` 调用该函数的动作

- 动作在 `createActions()` 方法中创建。
- 将该动作插入 `createMenuBar()` 方法中的 Functions 菜单。
- 将该动作插入 `createToolBars()` 方法中的 Functions 工具栏。

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

#### 现在新的空函数应该已经可用了

- 如果光学系统（schema）为空，该函数不会被调用。在调用该函数之前，至少先添加一个元件。
- 该函数窗口已经可以保存在 schema 文件中，并将被恢复。
