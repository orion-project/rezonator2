.. _custom_funcs:

自定义函数
================

自定义函数提供了一种强大的方式来扩展 |rezonator| 的功能：你可以编写自己的 Python 代码来分析光路、执行专门的计算，并以适合你具体需求的方式可视化结果。

自定义函数有三种类型，每种都为不同的输出格式而设计：

- :doc:`Custom Script <custom_script>` —— 用于通用计算，文本输出到日志面板
- :doc:`Custom Table <custom_table>` —— 用于以结构化表格格式呈现的计算
- :doc:`Custom Plot <custom_plot>` —— 用于以图形和绘图形式可视化的计算

所有自定义函数都使用同一个 :doc:`Python API <py_api>` 与你的光路交互、访问元件属性并执行计算。

自定义函数类型概述
----------------------------------

Custom Script
~~~~~~~~~~~~~

:menuselection:`Functions --> New Custom Script Window`

:doc:`custom_script` 函数是自定义函数最简单的形式，为带文本输出的任意计算而设计。它非常适合：

- 快速计算和调试
- 生成自定义报告
- 执行一次性分析
- 为更复杂的函数做原型

结果使用 :ref:`rezonator.print() <py_module_z_print>` 函数显示在脚本窗口底部的日志面板中。

  .. .. note::
  ..   Script code is stored only in the editor window and will be lost when the window is closed. Save your scripts to the :ref:`custom code library <custom_func_lib>` for reuse.

Custom Table
~~~~~~~~~~~~

:menuselection:`Functions --> Create Custom Table Function`

:doc:`custom_table` 函数以结构化的 :doc:`表格格式 <table_window>` 呈现计算结果。它非常适合：

- 比较多个元件的属性
- 计算光路沿线不同位置处的值
- 显示带自动单位换算的结果
- 呈现适合用表格组织的数据

该函数会为每个元件和位置自动调用你的代码，构建一个全面的结果表。

  .. .. note::
  ..   Table code is stored in the table window and persists when you close the code editor. The function is lost only when you close the table window itself.

Custom Plot
~~~~~~~~~~~

:menuselection:`Functions --> Create Custom Plot Function`

:doc:`custom_plot` 函数把计算结果可视化为 :doc:`绘图窗口 <plot_window>` 中的图形。它非常适合：

- 可视化依赖关系和趋势
- 参数扫描分析
- 光束传播可视化
- 稳定性图和其他图形分析

该函数生成数据点，这些数据点会被自动绘制，并完整支持多条图形线、单位换算和交互式绘图控件。

  .. .. note::
  ..   Plot code is stored in the plot window and persists when you close the code editor. The function is lost only when you close the plot window itself.

共同特点
---------------

所有自定义函数类型都共享几个共同功能：

Python API 访问
~~~~~~~~~~~~~~~~~

所有自定义函数都使用同一个全面的 :doc:`py_api`，它提供：

- 通过 :doc:`Element <py_element>` 对象访问光路元件
- 通过 :doc:`RoundTrip <py_round_trip>` 对象进行往返计算
- 通过 :doc:`Matrix <py_matrix>` 和 :doc:`Matrix3 <py_matrix3>` 对象进行矩阵运算
- 通过 :doc:`Schema <py_schema>` 对象进行光路级别的访问
- 全局 :doc:`rezonator 模块 <py_global>` 中的实用函数

自定义参数
~~~~~~~~~~~~~~~~~

:ref:`自定义参数 <custom_params>` 允许你在任何元件的内置参数之外，用附加的用户定义参数来扩展它。这对自定义函数特别有用，因为你可以：

- 保存中间计算结果
- 跟踪你的分析特有的属性
- 添加元件特定的配置数据
- 为默认未涵盖的属性创建参数

自定义参数通过 :ref:`Element Properties 对话框 <elem_props>` 添加，并在你的代码中使用 :ref:`Element.param() <py_method_elem_param>` 方法访问。

代码库
~~~~~~~~~~~~

所有自定义函数都可以使用 :menuselection:`Script --> Save to Custom Library` 保存到 :ref:`自定义代码库 <custom_func_lib>`。这使你可以：

- 建立一个可复用计算脚本的集合
- 在不同光路之间共享分析代码
- 维护一个常用计算的库
- 从模板快速创建新函数

该库同时保存代码和元数据（函数名称、说明、创建日期），以便于组织和检索。

SI 单位
~~~~~~~~

|rezonator| 中的所有计算内部都使用 SI 单位（米、弧度等）：

- 参数值始终以 SI 单位提供
- 对于 **自定义脚本**：你应当手动格式化输出以提高可读性（例如把米转换为毫米）
- 对于 **自定义表格和绘图**：以 SI 单位返回值；界面会根据量纲类型自动处理单位换算

选择合适的函数类型
---------------------------------

选择自定义函数类型时，考虑以下几点：

当你需要以下功能时，使用 **Custom Script**：
  - 简单的基于文本的输出
  - 快速计算或调试
  - 自定义报告或摘要
  - 无需预定义输出格式的最大灵活性

当你需要以下功能时，使用 **Custom Table**：
  - 多个值的结构化呈现
  - 跨元件或位置的比较
  - 界面中的自动单位换算
  - 有组织的表格数据显示

当你需要以下功能时，使用 **Custom Plot**：
  - 结果的图形可视化
  - 趋势分析或参数扫描
  - 带缩放和平移的交互式绘图
  - 同一坐标轴上的多条图形线

入门
---------------

1. **学习 Python API**：查阅 :doc:`py_api` 文档，了解可用的对象和方法

2. **学习示例**：通过 :menuselection:`File --> Open Example` 打开示例工程：

  - ``custom_func_simple.rez`` —— 通用编码示例
  - ``custom_func_aper_ratio.rez`` —— 最简单的自定义表格示例
  - ``custom_plot.rez`` —— 最简单的自定义绘图示例
  - ``misalign.rez`` —— 自定义表格和绘图结合的示例

3. **从简单开始**：从 :doc:`custom_script` 开始理解 API，然后再进阶到表格和绘图

4. **使用自定义参数**：当你需要保存附加数据时，用 :ref:`自定义参数 <custom_params>` 扩展元件

5. **建立你的库**：把成功的函数保存到 :ref:`自定义代码库 <custom_func_lib>` 以供复用

.. seeAlso::

  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :doc:`custom_params`
  - :doc:`custom_func_lib`
  - :doc:`py_api`
  - :doc:`py_examples`
