.. _elem_opers_formula:

编辑公式
============

:menuSelection:`Element --> Edit Formula`

该命令仅对 :doc:`matrix/ElemFormula` 处于活动状态。该命令打开一个代码编辑器窗口，用于更改该元件的矩阵计算代码。除非应用到元件上，否则更改后的代码只存在于该窗口内，正如窗口标题中所说明的那样。如果你关闭窗口，所有更改都会丢失。使用窗口工具栏上的 :guiLabel:`Apply` 按钮或菜单命令 :menuSelection:`Formula --> Apply Formula`，将代码保存到源元件中并重新计算其矩阵。

该窗口不允许修改代码中用到的元件参数。为此，请使用 :doc:`elem_props` 对话框及其编辑 :doc:`custom_params` 的功能。

  .. image:: img/elem_opers_formula.png

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`custom_funcs`
