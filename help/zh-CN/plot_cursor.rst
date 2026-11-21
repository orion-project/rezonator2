.. _cursor:
.. index:: single: cursor
.. index:: single: plot cursor

绘图光标
===========

:menuSelection:`View --> Cursor`

光标是一条或两条交叉线，可以用鼠标拖动。光标面板显示光标交叉点的坐标，以及在光标所在点计算出的函数值（在 T 和 S 平面中）。显示值的精度由 :ref:`应用设置 <app_settings_format_precision>` 控制。

光标下的函数值是精确计算的，因此它们不是由图中已有的点插值得到的。一个例外是 :ref:`冻结的 <func_freeze>` 函数，此时光标下的函数值通过图形相邻点之间的插值计算。

当你把图作为图像 :ref:`复制 <plot_opers_copy_image>` 到剪贴板时，光标线可能是不需要的。与其手动打开和关闭它们，不如使用 :ref:`应用选项 <app_settings_export_hide_cursor>`，它会自动完成这一操作。

  .. image:: img/plot_cursor.png

类型
----

:menuSelection:`View --> Cursor --> Vertical Line`

:menuSelection:`View --> Cursor --> Horizontal Line`

:menuSelection:`View --> Cursor --> Both Lines`

你可以使用光标面板左侧的 :menuSelection:`Cursor` 按钮快速切换光标的可见性。光标类型可以是一条竖线、一条横线，或两条线。你可以在光标切换按钮下方的下拉菜单中选择所需的类型。

  .. image:: img/plot_cursor_dropdown.png

定位
-----------

除了用鼠标拖动它之外，你还可以精确设置光标的位置。为此，双击光标面板中的 X 或 Y 坐标，在对话框中输入所需的值，然后按 Enter 键。

  .. image:: img/plot_cursor_set_pos.png

跟随鼠标
~~~~~~~~~~~~

:menuSelection:`View --> Cursor --> Follow Mouse (F7)`

除了菜单选项外，光标面板右侧也有相应的按钮。当该选项打开时，光标会即时跟随鼠标移动，函数值会立即重新计算。双击即可停止跟随。

外观
----------

:menuSelection:`Format --> Cursor Lines Format...`

光标线的默认外观由 :ref:`程序偏好设置 <app_settings_lines>` 决定。它应用于所有新打开的绘图窗口，以及那些尚未覆盖光标格式的窗口。默认外观可以通过菜单命令覆盖，并存储在光路项目文件中。

.. seeAlso::

  - :doc:`plot_window`
