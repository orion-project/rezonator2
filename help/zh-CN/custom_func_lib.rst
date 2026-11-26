.. _custom_func_lib:

自定义函数库
=========================

:menuSelection:`Functions --> Custom Functions Library`

自定义函数库为你可复用的自定义函数提供集中管理。它允许你保存、组织和复用跨不同光路的自定义函数代码，建立一个为你的具体需求量身定制的个人计算工具集合。

自定义函数以 Python 文件的形式保存在用户配置文件目录中。每个文件都被赋予一个便于用户阅读的名称，该名称从脚本的文档字符串中提取。为你的脚本选择描述性强、有意义的名称，以便以后容易查找和识别它们。

例如，这个函数在库中将显示为 “Beam Waist Calculator”：

.. code-block:: python

  """
  Beam Waist Calculator

  Calculates the beam waist position and size
  for a given cavity configuration.
  """

  def calculate():
    # Your code here
    pass

库窗口在一个列表中显示所有已保存的自定义函数文件，并用图标指示它们的类型：

+---------------+------------------------------------------------------------------------------+
|               |                                                                              |
+===============+==============================================================================+
| |icon_python| | 用于 :doc:`文本输出 <custom_script>` 的基本自定义脚本                        |
+---------------+------------------------------------------------------------------------------+
| |icon_table|  | 具有 ``columns()`` 方法、用于 :doc:`表格输出 <custom_table>` 的函数          |
+---------------+------------------------------------------------------------------------------+
| |icon_plot|   | 具有 ``figure()`` 方法、用于 :doc:`图形输出 <custom_plot>` 的函数            |
+---------------+------------------------------------------------------------------------------+

.. |icon_python| image:: ../img/toolbar/func_script.svg
.. |icon_table| image:: ../img/toolbar/func_table.svg
.. |icon_plot| image:: ../img/toolbar/func_plot.svg

点击列表中的任何文件，即可在右侧面板中查看它的代码。代码查看器默认是只读的，以防止意外的修改。

过滤
------

使用文件列表顶部的过滤字段可以快速查找自定义函数。你可以按 :guiLabel:`F3` 快捷键快速激活该字段。你可以输入多个以空格分隔的词（每个词至少 2 个字符）。只有名称包含所有搜索词的函数才会被显示（AND 逻辑）。搜索不区分大小写。例如，输入 “beam plot” 将只显示名称中同时包含 “beam” 和 “plot” 的函数。


编辑
----

对于小的修正和拼写错误，你可以直接在窗口中编辑文件。使用工具栏上的 :guiLabel:`Edit` 按钮或按 :guiLabel:`F2` 快捷键。在代码编辑器中进行更改，然后点击 :guiLabel:`Save`（:guiLabel:`Ctrl+S`）保存更改，或点击 :guiLabel:`Cancel`（:guiLabel:`Esc`）放弃更改。

对于常规开发，你应当通过 :guiLabel:`Apply to Schema` 命令把函数添加到一个真实的光路中，在那里修改和测试它，以验证它能正确工作。然后根据函数类型，使用 :menuSelection:`Script`、:menuSelection:`Table` 或 :menuSelection:`Plot` 菜单中的 :menuSelection:`Save to Custom Library` 菜单命令把它保存回库中。这种工作流程能确保你的脚本在更新库之前已用实际数据妥善测试过。

创建
------

要向库中添加一个新的自定义函数，编写一个新的 :doc:`custom_script` 或 :doc:`custom_table` 或 :doc:`custom_plot`。添加一个第一行是便于人阅读的名称的文档字符串。然后根据函数类型，使用 :menuSelection:`Script`、:menuSelection:`Table` 或 :menuSelection:`Plot` 菜单中的 :menuSelection:`Save to Custom Library` 菜单命令。

如果库中已经存在同名的函数，系统会询问你是否替换它。库假定每个自定义功能都应当有唯一的命名。如果有人重复使用一个名称，那他很可能是想重新实现该功能。

删除
------

要从库中删除一个自定义函数，按 :guiLabel:`Delete` 按钮。从库中删除一个函数不会影响任何使用它的光路。现有的光路保留它们自己的代码副本。

导入/导出
-------------

自定义函数以 Python 文件的形式保存在用户配置文件目录中。使用 :guiLabel:`Export` 和 :guiLabel:`Import` 按钮来共享和备份。导入的文件必须是 UTF-8 编码的 Python 文件，包含一个第一行是函数名称的文档字符串和 ``calculate()`` 函数。

.. seeAlso::

  - :doc:`functions`
  - :doc:`custom_script`
  - :doc:`custom_table`
  - :doc:`custom_plot`
  - :doc:`py_api`
