自定义绘图函数
====================

:menuSelection:`Functions --> Create Custom Plot Function`

自定义绘图函数允许你使用提供的 :doc:`py_api` 进行任意的用户计算，并把结果作为图形输出到 :doc:`绘图窗口 <plot_window>` 中，而不是像更简单的 :doc:`custom_script` 函数那样以文本形式输出。这个强大的功能使你能够分析你的光路、执行自定义计算，并以图形方式可视化结果，非常适合可视化光学参数之间的依赖关系、趋势和关系。

对于自定义函数，绘图窗口会显示一个附加命令 :menuSelection:`Plot --> Show Code`，以及工具栏上带 Python 图标的相应按钮。该命令打开一个代码编辑器窗口，用于更改你的脚本代码。这个窗口可以安全关闭，因为代码实际上保存在绘图窗口中，而不是代码窗口中。但是，如果你关闭绘图窗口本身，函数代码 *将会丢失*。

代码模块
-----------

自定义函数是一个用户定义的 Python 模块。模块文档字符串的第一行将被用作便于人阅读的函数名称，并显示在函数窗口的标题中。这使你可以为你的自定义绘图函数起一个有意义的标题。在绘图的标题中，这个名称将被替换到 ``{func_name}`` 文本变量的位置。

``figure()``
~~~~~~~~~~~~

代码模块必须包含一个 ``figure()`` 函数，它配置绘图的属性。这个函数在每次重新计算之前被调用，这使你可以动态更改绘图属性（例如根据选中的元件更改坐标轴标题），而无需重新打开窗口。

.. code-block:: python

  def figure():
    return {
      'x_dim': Z.DIM_LINEAR,
      'x_title': 'Position',
      'y_dim': Z.DIM_LINEAR,
      'y_title': 'Beam radius'
    }

该函数应当返回一个包含以下键的字典：

- ``x_dim`` —— X 轴的量纲（:ref:`Z.DIM_LINEAR <py_const_dim_linear>`、:ref:`Z.DIM_ANGULAR <py_const_dim_angular>`、:ref:`Z.DIM_NONE <py_const_dim_none>`）
- ``x_title`` —— X 轴的默认标题。它可以包含元件标签或参数说明，并将被替换到 ``{default_title}`` 文本变量的位置。
- ``y_dim`` —— Y 轴的量纲
- ``y_title`` —— Y 轴的默认标题

``calculate()``
~~~~~~~~~~~~~~~

代码还必须包含一个 ``calculate()`` 函数，它执行实际的计算并生成要绘制的数据。当你按下 :guilabel:`Run` 按钮，或使用 :menuSelection:`Script --> Run` 菜单命令，或通过 :menuSelection:`Plot --> Update` 刷新绘图窗口时，这个函数会被无参数地调用。

.. code-block:: python

  def calculate():
    # 执行计算
    x_values = [...]
    y_values = [...]
    return [
      {
        'label': 'My graph',
        'x': x_values,
        'y': y_values
      }
    ]

该函数应当返回一个图形线条数据的列表，其中每条线都表示为一个带以下键的字典：

- ``label`` —— 显示在图例中的名称；同时，它也是把几条线合并为单条多段图形的键
- ``x`` —— X 坐标值的列表或数组
- ``y`` —— Y 坐标值的列表或数组（必须与 ``x`` 长度相同）

输出
~~~~~~

|rezonator| 中的所有计算都以 SI 单位进行（米、弧度等）。提供给脚本的所有参数值也都是 SI 单位。对于自定义绘图函数，你应当以 SI 单位返回计算出的 X 和 Y 值，不做任何换算。因为绘图窗口具有内置功能，可以根据坐标轴的量纲类型（在 ``figure()`` 函数中定义的 ``x_dim`` 和 ``y_dim`` 属性）选择和显示单位。用户可以通过绘图窗口界面选择首选单位。

``calculate()`` 函数的结果会被自动显示为 :doc:`plot_window` 中的图形。你可以返回多个字典，在同一张图上显示几条线。具有相同 ``label`` 的字典会成为单条多段图形的组成部分。对于函数未定义或谐振腔不稳定的 Y 值，你可以使用 NaN（写作 ``float('nan')``）。一条包含 NaN 值的线会被拆分为几段，也成为多段图形。线条标签 “T” 和 “S” 会被额外处理，从而应用一种用于切向平面和弧矢平面的特殊 :ref:`图形格式 <app_settings_lines>`。

对于简单的文本输出或调试，使用 :doc:`custom_script`。对于表格数据呈现，考虑使用 :doc:`custom_table`。

.. _custom_plot_lock:

元件锁定
---------------

当你的计算会改变元件参数时（例如在参数扫描中），你应当在修改元件之前锁定它，之后再解锁它。这种做法：

- 保留原始参数值以便自动恢复
- 在计算期间禁用界面更新，以获得更好的性能
- 在参数改变时防止其他函数重新计算

.. code-block:: python

  elem.lock()
  try:
    for i in range(point_count):
      elem.set_param('R', value)
      # 执行计算...
  finally:
      elem.unlock()

始终使用 try/finally 块，以确保即使发生错误，元件也能被解锁。

一种更方便的方法是使用 Python 的 ``with`` 块，以及专门的 :ref:`Element.param_ref() <py_method_elem_param_ref>` 或 :ref:`schema.param_ref() <py_method_schema_param_ref>` 方法，它们返回 :doc:`ParamRef <py_param_ref>` 对象，允许自动锁定/解锁和恢复参数值：

.. code-block:: python

  with elem.param_res('R') as r:
    for i in range(point_count):
      r.set_value(value)
      # 执行计算...

自定义库
--------------

你可以使用 :menuSelection:`Script --> Save to Custom Library` 菜单命令，把你的代码保存到 :ref:`自定义代码库 <custom_func_lib>` 中以备后用。这使你可以为常见任务建立一个可复用计算脚本的集合

自定义参数
-----------------

自定义参数是你可以添加到任何元件的任意参数，是对它预定义参数的补充。它们对自定义表格函数特别有用，因为它们允许你计算或跟踪元件默认参数未涵盖的属性。

你可以通过元件属性对话框给元件添加自定义参数，然后在计算中使用 :ref:`Element.param()<py_method_elem_param>` 方法访问它们。这提供了灵活性，可以用项目特定的数据或你的自定义计算特有的属性来扩展元件。

示例工程
----------------

通过 :menuSelection:`File --> Open Example` 打开完整可用的示例 ``custom_plot.rez`` 和 ``misalign.rez``，看看自定义绘图函数在实践中如何工作，并把它用作你自己计算的模板。在 Open Example 对话框中使用 “custom plot” 过滤器，查看该主题的可用示例。

.. seeAlso::

  - :doc:`py_api`
  - :doc:`plot_window`
  - :doc:`custom_funcs`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :ref:`示例 - 自定义焦散函数 <py_example_custom_caustic>`
