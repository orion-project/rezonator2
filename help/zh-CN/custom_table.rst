自定义表格函数
=====================

:menuSelection:`Functions --> Create Custom Table Function`

自定义表格函数允许你使用提供的 :doc:`py_api` 进行任意的用户计算，并把结果输出到 :doc:`表格视图 <table_window>` 中，而不是像更简单的 :doc:`custom_script` 函数那样以文本形式输出。这个强大的功能使你能够分析你的光路、执行自定义计算，并以结构化的表格格式呈现结果，非常适合比较多个元件或位置的值。

对于自定义函数，表格窗口会显示一个附加命令 :menuSelection:`Table --> Show Code`，以及工具栏上带 Python 图标的相应按钮。该命令打开一个代码编辑器窗口，用于更改你的脚本代码。这个窗口可以安全关闭，因为代码实际上保存在表格窗口中，而不是代码窗口中。但是，如果你关闭表格窗口本身，函数代码 *将会丢失*。

代码模块
-----------

自定义函数是一个用户定义的 Python 模块。模块文档字符串的第一行将被用作便于人阅读的函数名称，并显示在函数窗口的标题中。这使你可以为你的函数起一个有意义的标题。

``columns()``
~~~~~~~~~~~~~

代码模块必须提供一个 ``columns()`` 函数，它定义表格列的结构和属性。这个函数在每次重新计算之前被调用，这使你可以动态更改表格结构（例如根据选中的元件或参数更改列标题），而无需重新打开窗口。

.. code-block:: python

  def columns():
    return [
      {
        'label': 'W',
        'title': 'Beam radius',
        'dim': Z.DIM_LINEAR
      },
      {
        'label': 'Ratio',
        'title': 'Aperture ratio',
        'dim': Z.DIM_NONE
      }
    ]

每个列定义是一个字典，包含以下键：

- ``label`` —— 计算值的简短名称；它显示在列标题中，也用于从 ``calculate()`` 函数返回的结果字典中提取值
- ``title`` —— 完整的描述性标题，显示在表格窗口的菜单中
- ``dim`` —— 量纲类型（:ref:`Z.DIM_LINEAR <py_const_dim_linear>`、:ref:`Z.DIM_ANGULAR <py_const_dim_angular>`、:ref:`Z.DIM_NONE <py_const_dim_none>`），在为该列选择计量单位时使用

``calculate()``
~~~~~~~~~~~~~~~

代码还必须包含一个 ``calculate()`` 函数，它执行实际的计算。对于表格函数，这个函数会被多次调用——每个元件至少两次（分别针对 T 平面和 S 平面），并且根据表格窗口中选择的 :ref:`选项 <table_opers_calc_at_medium_ends>`，可能会对同一个元件调用 :doc:`多次 <table_symbols>`。

.. code-block:: python

  def calculate(elem, pos, rt):
    # 对给定的元件和位置执行计算
    if pos == POS_RIGHT:
      w = rt.beam_radius()
      return {'W': w}
    return None

该函数接收三个参数：

- ``elem`` —— 当前正在处理的 :doc:`Element <py_element>` 对象
- ``pos`` —— 位置指示符，可能的值有：

  - ``POS_LEFT`` —— 元件的左侧（在其之前）
  - ``POS_BEG`` —— 元件内部的起始处
  - ``POS_MID`` —— 元件内部的中间
  - ``POS_END`` —— 元件内部的末端
  - ``POS_RIGHT`` —— 元件的右侧（在其之后）

- ``rt`` —— 为当前位置预先计算的 :doc:`RoundTrip <py_round_trip>` 对象。:ref:`RoundTrip.plane <py_method_rt_plane>` 属性显示该函数是为哪个工作平面调用的。

该函数应当返回：

- 一个字典，其键与 ``columns()`` 中定义的列标签相匹配，或者
- ``None``，如果这个元件/位置组合不应显示任何数据

输出
~~~~~~

|rezonator| 中的所有计算都以 SI 单位进行（米、弧度等）。提供给脚本的所有参数值也都是 SI 单位。对于自定义表格函数，你也应当以 SI 单位返回计算值，不做任何换算。因为表格窗口具有内置功能，可以根据列的量纲类型（``dim`` 属性）选择和显示 :ref:`单位 <table_opers_units>`。

``calculate()`` 函数的结果会被自动格式化并显示在 :doc:`table_window` 中。表中的每一行对应一个元件或位置，每一列显示你的 ``columns()`` 函数定义的计算值。

对于简单的文本输出或调试，改用 :doc:`custom_script`。对于图形可视化，考虑使用 :doc:`custom_plot`。

自定义库
--------------

你可以使用 :menuSelection:`Script --> Save to Custom Library` 菜单命令，把你的代码保存到 :ref:`自定义代码库 <custom_func_lib>` 中以备后用。这使你可以为常见任务建立一个可复用计算脚本的集合。

自定义参数
-----------------

自定义参数是你可以添加到任何元件的任意参数，是对它预定义参数的补充。它们对自定义表格函数特别有用，因为它们允许你计算或跟踪元件默认参数未涵盖的属性。

你可以通过元件属性对话框给元件添加自定义参数，然后在计算中使用 :ref:`Element.param()<py_method_elem_param>` 方法访问它们。这提供了灵活性，可以用项目特定的数据或你的自定义计算特有的属性来扩展元件。

示例工程
----------------

通过 :menuSelection:`File --> Open Example` 打开完整可用的示例 ``custom_func_aper_ratio.rez`` 和 ``misalign.rez``，看看自定义表格函数在实践中如何工作，并把它用作你自己计算的模板。在 Open Example 对话框中使用 “custom table” 过滤器，查看该主题的可用示例。

.. seeAlso::

  - :doc:`py_api`
  - :doc:`table_window`
  - :doc:`custom_funcs`
  - :doc:`custom_script`
  - :doc:`custom_plot`
  - :ref:`示例 - 光阑比计算 <py_example_aper_ratio>`
