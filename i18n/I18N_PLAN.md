# reZonator 界面汉化（i18n）实施计划

> 范围：只针对软件界面（Qt Widgets UI）。README 与帮助文档的汉化是另一条线，不在本计划内。
> 本计划基于对仓库 `src` 的只读统计（未改动仓库任何文件），统计日期 2026-11-20，仓库版本以当前 fork 的 `aa7e527` 为准。

## 1. 现状统计

| 项目 | 数值 | 说明 |
|---|---|---|
| `src` 源文件（.cpp/.h） | 353 个 | 不含 `libs/orion`、`libs/custom-plot-lab` 两个子库 |
| `tr(` 调用（词边界匹配） | 890 处 | `grep -Roh '\btr('` 的结果；任务背景中“约 1079 处”是未加词边界、把 `str(`、`.str()` 也算进去的粗计数，应以 890 为准 |
| `tr("...")` 字面量调用 | 887 处 | 其余 3 处是 `tr(变量)`，见 §5.3 |
| **`tr()` 去重后英文字符串** | **617 条** | 完整清单见同目录 `ui-strings.txt`（一行一条，保留源码中的 `\n` 等转义写法） |
| `translate("上下文", "...")` 调用 | 232 处，去重 178 条 | 其中 166 条不在 `tr()` 集合里，是额外待译量，见 §5.2 |
| 合并去重待译字符串 | 783 条 | 617 + 166，不含任何硬编码字符串 |
| 已有 `.ts` / `.qm` 文件 | 0 个 | 仓库里从未建立翻译流程 |
| `main.cpp` 中的 QTranslator | 无 | 没有加载翻译的代码 |
| `CMakeLists.txt` 中的 LinguistTools | 无 | `find_package` 只引入了 Help、PrintSupport、Svg、Widgets |
| AppSettings 中的语言选项 | 无 | 只有与导出有关的 `exportSystemLocale`，与界面语言无关，需要新增 |

### tr() 字符串最多的前 20 个文件（调用处数 / 该文件去重条数）

| # | 调用处数 | 去重条数 | 文件 |
|---|---|---|---|
| 1 | 84 | 68 | funcs/PlotFuncWindow.cpp |
| 2 | 78 | 78 | windows/ProjectWindow.cpp |
| 3 | 62 | 51 | windows/PlotBaseWindow.cpp |
| 4 | 56 | 54 | windows/AppSettingsDialog.cpp |
| 5 | 37 | 31 | windows/SchemaViewWindow.cpp |
| 6 | 34 | 32 | tools/LensmakerWindow.cpp |
| 7 | 31 | 30 | windows/MemoWindow.cpp |
| 8 | 29 | 24 | funcs/StabilityMap2DWindow.cpp |
| 9 | 27 | 26 | tools/GaussCalculatorWindow.cpp |
| 10 | 27 | 26 | windows/StartWindow.cpp |
| 11 | 24 | 22 | windows/TextEditDialog.cpp |
| 12 | 23 | 21 | windows/CustomFuncsWindow.cpp |
| 13 | 20 | 20 | windows/ElementPropsDialog.cpp |
| 14 | 20 | 20 | windows/PumpWindow.cpp |
| 15 | 16 | 15 | funcs/TableFuncWindow.cpp |
| 16 | 15 | 14 | funcs/StabilityMapWindow.cpp |
| 17 | 15 | 15 | windows/AdjustmentWindow.cpp |
| 18 | 15 | 15 | windows/CustomElemsWindow.cpp |
| 19 | 14 | 14 | windows/SchemaParamsWindow.cpp |
| 20 | 13 | 13 | tools/CalculatorWindow.cpp |

前 20 个文件占全部 887 处调用的约 64%，其余字符串长尾分布在其余文件里。

## 2. 第一批已完成（本目录交付物）

- `ui-strings.txt`：617 条去重后的 `tr()` 英文字符串全量清单。
- `rezonator_zh_CN.ts`：初步 Qt 翻译文件（TS 2.1，`language="zh_CN"`，已通过 XML 解析校验），共 **204 条已译**：
  - `tr()` 第一批 200 条，选取顺序为 ProjectWindow（主窗口菜单/工具栏/状态栏，78 条）→ StartWindow（起始窗口，21 条）→ ElementsCatalogDialog（3 条）→ AppSettingsDialog（设置对话框，54 条）→ SchemaViewWindow（光路图元件操作，27 条）→ PlotBaseWindow（绘图窗口前 17 条）；
  - 另加 `core/ElementsCatalog.cpp` 用 `translate("Elements", ...)` 写的 4 个元件分类名（常用元件 / 附加元件 / 介质与界面 / 复合元件），上下文为 `Elements`。
  - 上下文（context）按文件主类名归一：ProjectWindow、StartWindow、ElementsCatalogDialog、AppSettingsDialog、SchemaViewWindow、PlotBaseWindow、Elements，共 7 个。每条都带 `<location>`（相对路径与行号）方便回查。
  - 注意：这份 .ts 是人工整理的初步版，正式接入后必须用 `lupdate` 重新提取校准上下文——源码里不少 `tr()` 实际写在子面板类（如 StartWindow 的 MruStartPanel、TipsStartPanel、ActionsStartPanel）或辅助类的方法里，`lupdate` 会按真实的元对象类名生成上下文，届时以 `lupdate` 的结果为准合并本文件的译文。

## 3. 给 main.cpp 加 QTranslator（具体步骤）

`src/main.cpp` 目前在 `QApplication app(argc, argv);` 之后直接设置应用名、样式，没有任何翻译加载。改法：

1. 在文件头部增加 `#include <QTranslator>` 和 `#include <QLocale>`。
2. 在 `QApplication app` 创建之后、创建任何窗口（尤其是 `StartWindow`、`ProjectWindow`）之前插入加载逻辑。翻译必须在这一步装好，晚了已创建的界面不会自动刷新（本应用没有运行时动态切换语言的需求，不要做 `changeEvent` 重翻译）。
3. 语言来源优先级（从高到低）：命令行参数 `--lang zh_CN`（建议新增，方便测试）→ AppSettings 新增的 `uiLanguage` 设置项 → `QLocale::system()`。找不到对应 `.qm` 时静默回退英文，不要弹窗。
4. 加载路径两处都试：资源内 `:/translations/rezonator_zh_CN.qm`（推荐，见 §4 第 3 步）和安装目录 `app.applicationDirPath() + "/translations/..."`（方便用户替换译文调试）。
5. 同时加载 Qt 自带翻译 `qtbase_zh_CN`（控件默认按钮如 OK/Cancel、文件对话框等靠它），路径用 `QLibraryInfo::path(QLibraryInfo::TranslationsPath)`。

参考代码（示意，接入时按项目代码风格调整）：

```cpp
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

// 在 QApplication 创建后、任何窗口创建前：
const QString uiLang = /* --lang 参数 > AppSettings.uiLanguage > QLocale::system().name() */;
if (!uiLang.isEmpty() && !uiLang.startsWith("en")) {
    auto *qtTranslator = new QTranslator(&app);
    if (qtTranslator->load("qtbase_" + uiLang, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app.installTranslator(qtTranslator);
    auto *appTranslator = new QTranslator(&app);
    const QString qmName = "rezonator_" + uiLang;
    if (appTranslator->load(qmName, ":/translations") ||
        appTranslator->load(qmName, app.applicationDirPath() + "/translations"))
        app.installTranslator(appTranslator);
}
```

6. AppSettings（`src/app/AppSettings.h/.cpp`）按现有 `LOAD_DEF/SAVE` 模式新增 `uiLanguage` 字符串设置项，并在 AppSettingsDialog 的 Interface 页加一个语言下拉框（跟随系统 / English / 简体中文），改后提示重启生效即可。

## 4. 给 CMake 加 LinguistTools（具体步骤）

`CMakeLists.txt` 第 49–50 行现在是：

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Help PrintSupport Svg Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Help PrintSupport Svg Widgets)
```

改法（项目同时兼容 Qt5/Qt6，两种分支都要处理）：

1. 两个 `find_package` 的 COMPONENTS 都加上 `LinguistTools`。
2. 在源码目录新建 `src/translations/rezonator_zh_CN.ts`（以本目录交付的 .ts 为初稿放进去），并定义：

```cmake
set(TS_FILES src/translations/rezonator_zh_CN.ts)
```

3. Qt6 分支：在 `qt_add_executable(...)` 之后、现有的 `qt_finalize_executable` 附近加：

```cmake
qt_add_translations(${PROJECT_NAME} TS_FILES ${TS_FILES} RESOURCE_PREFIX "/translations")
```

   这一条会同时在构建时跑 `lupdate`/`lrelease` 并把 `.qm` 嵌入资源，正好对应 §3 的 `:/translations` 路径。注意：`qt_add_translations` 默认会在构建时更新 .ts 文件（回写源码目录），CI 上要留意工作区变脏的问题，可改用 `qt_add_lrelease` + 手动 `lupdate` 的组合。
4. Qt5 分支：用旧式命令：

```cmake
qt5_add_translation(QM_FILES ${TS_FILES})
# 把 ${QM_FILES} 加入目标源文件，并另写一个 translations.qrc 把 .qm 以 /translations 前缀嵌入
```

5. 日常维护流程固定为：改完界面代码 → 跑 `lupdate`（或构建一次）→ 在 Qt Linguist 里补译新增/模糊（fuzzy）条目 → 构建自动 `lrelease`。把这一步写进 `docs/build.md`。

## 5. 硬编码字符串改 tr()（具体步骤）

### 5.1 直接写死在控件调用里的字面量

用 `grep -RnE '(setText|setWindowTitle|setTitle|QPushButton|QAction|addAction|setToolTip)\s*\(\s*"' src` 做第一轮粗筛（本轮粗筛命中 27 处调用点），逐个改成成员 `tr("...")` 或 `QObject::tr("...")`。已确认的典型例子在 `src/app/Appearance.cpp`（样式表编辑器，本身不在任何窗口类里，要用 `QObject::tr`）：

| 行号 | 现状 | 改法 |
|---|---|---|
| 244、285 | `new QPushButton("Apply")` | `QObject::tr("Apply")` → 应用 |
| 249、290 | `new QPushButton("Save")` | 与已有 `tr("Save")` 同译 → 保存 |
| 271 | `setWindowTitle("Stylesheet Editor")` | `QObject::tr("Stylesheet Editor")` |
| 305 | `setWindowTitle("Style Sheet Editor")` | 同上，原文两种拼写要统一为一条 |

注意 Appearance、SchemaWindows 这类被任务列为优先的文件，`tr()` 计数反而是 0——它们的文字要么写死、要么只是转手显示别处传来的标题，汉化时要逐文件人工过，不能只看 tr 统计。

### 5.2 不要漏掉 translate() 那 178 条

`qApp->translate("上下文", "...")` 是另一套已在用的可翻译写法，集中在 `src/core/Elements.cpp`、`Element.h` 等文件，上下文有 Param（94 处）、Elements（35）、exportGraphsData（20）、Pump param（15）、Pump mode（12）、Calc error（12）等 17 个。这批字符串（元件名、参数名、参数说明）是用户在元件目录和参数编辑器里天天看到的，优先级不比菜单低。好消息是它们已经是可翻译写法，`lupdate` 能直接提取，不需要改代码，只需要翻译（本批已译其中 Elements 分类的 4 条）。

### 5.3 数据表里的字符串 + tr(变量)

`src/core/CommonTypes.cpp` 第 40–72 行把系统类型的提示文字写在静态数据表里（`"Standing wave system (SW)"`、`"Ring resonator (RR)"` 等），运行时经 `TripTypeInfo::toolTip()` 的 `qApp->tr(_info->toolTip)` 翻译（`tr(` 总数里那 3 处非字面量调用就是这类）。`lupdate` 提取不到数据表里的原文，改法是给这些字面量加 `QT_TR_NOOP("...")` 标记（只标记、不改变类型），`lupdate` 就能收录，运行时仍走原来的 `tr(变量)`。全仓应按 `tr\([^"]` 再筛一遍同类模式。

### 5.4 不要误译的字符串

以下字面量不是界面文字，一律不动：`objectName`、样式表/CSS、资源路径（`:/...`）、文件格式过滤器中的内部标识、JSON/INI 的 key、Python/Lua 代码片段、单位符号与公式、日志里给开发者的 `qWarning/qDebug` 输出（可单独评估，不要混进 UI 批次）。判断标准：这串文字会不会原样出现在用户界面上。

### 5.5 子库 libs/orion

`libs/orion`（Ori 辅助控件库）和 `libs/custom-plot-lab` 是子库，本轮统计未计入。Ori 里有大量对话框按钮、文件对话框等通用控件文字，最终成品的完整度取决于它。建议第二阶段单独给 orion 库做一轮同样的 tr 化 + 自己的 .ts，不要和主程序的 .ts 混在一起。

## 6. 剩余数量与分批计划

已译 204 条（合并去重口径下占 783 条的约 26%），**剩余 579 条**（tr 剩余 417 条 + translate 独有剩余 162 条），另加数量未精确统计的硬编码字符串（§5）。

| 批次 | 内容 | 约条数 | 备注 |
|---|---|---|---|
| 第 1 批（已完成） | 主窗口菜单、起始窗口、设置、元件目录分类、光路图元件操作、绘图窗口开头 | 204 | 本目录 `rezonator_zh_CN.ts` |
| 第 2 批 | 绘图类窗口剩余：PlotBaseWindow 剩余约 34 条、PlotFuncWindow 68 条、StabilityMap / StabilityMap2D / TableFuncWindow 等 | 约 160 | 图轴、图例、光标等术语要先定表再翻 |
| 第 3 批 | 工具计算器：LensmakerWindow 32、GaussCalculatorWindow 26、CalculatorWindow 13、GrinLens 等 | 约 80 | 透镜、公式术语密集 |
| 第 4 批 | 其余对话框与窗口：MemoWindow 30、TextEditDialog 22、CustomFuncsWindow 21、ElementPropsDialog 20、PumpWindow 20、AdjustmentWindow 15、CustomElemsWindow 15、SchemaParamsWindow 14 等长尾 | 约 170 | 含大量确认框、报错文案，注意整句翻译 |
| 第 5 批 | translate() 专项：Param / Elements 元件与参数名、说明（§5.2 剩余部分） | 约 162 | 与元件目录、参数编辑器显示直接相关 |
| 第 6 批 | 硬编码清理：Appearance 等 §5 全部 + QT_TR_NOOP 数据表 + orion 子库评估 | 待统计 | 完成代码改造后用 lupdate 复查增量 |
| 收尾 | 全量校对：占位符 %1/%2、快捷键、数字/单位、繁简与标点统一；在 zh_CN 系统区域下实机跑一遍主流程截图核对 | — |  fuzzy 条目清零后再发版 |

每批的验收方式相同：该批 .ts 条目在 Linguist 里无未译、无 fuzzy，`lrelease` 无警告，实机打开对应窗口逐项对照。

## 7. 术语表（已定，翻译时统一执行）

| 英文 | 中文 | 说明 |
|---|---|---|
| laser resonator | 激光谐振腔 | |
| Gaussian beam | 高斯光束 | Gauss Calculator 译“高斯计算器” |
| beam waist | 束腰 | |
| stability map | 稳定性图 | 2D Stability Map 译“二维稳定性图” |
| optical element | 光学元件 | 界面里 Element 单独出现时译“元件” |
| round-trip (matrix) | 往返（矩阵） | |
| caustic | 焦散曲线 | 本软件指光束焦散曲线图 |
| schema | 光路图 | 本软件对光学系统布局的叫法，不译“模式/架构” |
| pump | 泵浦 | |
| File / Edit / View / Tools / Window / Help | 文件 / 编辑 / 视图 / 工具 / 窗口 / 帮助 | 菜单词用常见软件译法 |
| Settings | 设置 | Settings... 译“设置...” |
| Standing-Wave Resonator / Ring Resonator / Single-Pass System | 驻波谐振腔 / 环形谐振腔 / 单程系统 | 起始窗口的三种新建类型 |
| T-plane / S-plane | T 平面 / S 平面 | 子午/弧矢平面在界面上保留 T/S 写法，与原版图轴标注一致 |
| Elements Catalog | 元件目录 | |
| Custom Elements Library | 自定义元件库 | |

翻译通则：保留原文的 `...` 后缀与 `%1`、`%2` 占位符；按钮/菜单用短语，确认框与报错用完整句；原文明显拼写错误（如 `Beam radus`、`Beam Parameters at Elemens`）按正确含义翻译，并在回馈上游时单独提 issue，不在本汉化中改英文原文。
