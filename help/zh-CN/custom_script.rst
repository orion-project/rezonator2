自定义脚本
=============

:menuSelection:`Functions --> New Custom Script Window`

Custom Script 函数允许你使用提供的 :doc:`py_api` 进行任意的用户计算。这个强大的功能使你能够分析你的光路、执行自定义计算，并根据你的具体需求处理结果。

代码模块
-----------

自定义脚本是一个用户定义的 Python 模块。模块文档字符串的第一行将被用作便于人阅读的函数名称，并显示在函数窗口的标题中。这使你可以为脚本起一个有意义的标题。

  .. note::
    与 :doc:`表格 <custom_table>` 或 :doc:`绘图 <custom_plot>` 自定义函数不同——在那些函数中，脚本代码保存在相应的视图窗口之后——对于自定义脚本函数，脚本编辑器窗口是你的代码的唯一保存之处。所以如果你关闭编辑器窗口，函数代码 *将会丢失*。

``calculate()``
~~~~~~~~~~~~~~~

脚本必须包含一个 ``calculate`` 函数，当你按下工具栏上的 :guilabel:`Run` 按钮或使用 :menuSelection:`Script --> Run` 菜单命令时，它会被无参数地自动调用。这个方法是你的自定义计算的入口点，每次你运行脚本时都会被执行。

.. code-block:: python

  def calculate():
    # Your calculation code here
    pass

输出
~~~~~~

|rezonator| 中的所有计算都以 SI 单位进行（米、弧度等）。提供给脚本的所有参数值也都是 SI 单位。对于输出到日志面板的自定义脚本，你应当手动把值格式化为更方便的单位，以提高可读性。例如，在显示波长或光束尺寸时，把米转换为毫米或纳米。

自定义代码以文本形式把结果输出到脚本窗口底部的日志面板。这对于显示计算结果、调试信息或状态消息很有用。要在日志面板中看到输出，应使用 :ref:`rezonator.print() <py_module_z_print>` 辅助函数，而不是 Python 内置的 ``print()`` 函数。要清除日志面板，使用 :menuSelection:`Script --> Clear Log`。

对于不同种类的输出，可以考虑使用 :doc:`custom_table` 来呈现表格数据，或使用 :doc:`custom_plot` 来对结果进行图形可视化。

自定义库
--------------

你可以使用 :menuSelection:`Script --> Save to Custom Library` 菜单命令，把你的代码保存到 :ref:`自定义代码库 <custom_func_lib>` 中以备后用。这使你可以为常见任务建立一个可复用计算脚本的集合。

自定义参数
-----------------

:ref:`自定义参数 <custom_params>` 是你可以添加到任何元件的任意参数，是对它预定义参数的补充。它们对自定义脚本特别有用，因为它们允许你计算或跟踪元件默认参数未涵盖的属性。

你可以通过 :doc:`elem_props` 对话框给元件添加自定义参数，然后在自定义脚本中使用 :ref:`Element.param()<py_method_elem_param>` 方法访问和操作它们。这提供了灵活性，可以用项目特定的数据或中间计算结果来扩展元件。

.. seeAlso::

  - :doc:`py_api`
  - :doc:`custom_funcs`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :ref:`Example - Basic Ray Tracing<py_example_basic_ray_tracing>`
