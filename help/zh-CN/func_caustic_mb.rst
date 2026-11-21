.. index:: single: multibeam caustic (function)

多光束焦散
=================

:menuSelection:`Functions --> Multibeam Caustic`

这是 :doc:`func_caustic_mr` 针对 :ref:`单程 <schema_kind_sp>` 系统的一个特殊版本。它计算在 :doc:`pumps_window` 中设置的所有输入光束的光束半径。

使用 Pumps 窗口中的 :ref:`Set Color <pump_window_set_color>` 命令，可以为与某束输入光束对应的图线设置独特的外观。颜色更改会立即应用于所有已打开的函数窗口。

  .. image:: img/func_caustic_mb_colors.png

参数
----------

该函数具有与 :doc:`func_caustic_mr` 函数相同的参数。

附加参数
---------------------

该函数没有附加参数

特殊点
--------------

:menuSelection:`View --> Special Points`

该函数计算与 :ref:`焦散 <func_caustic_spec_points>` 函数相同的值，但对每个元件分别计算。值只针对与当前在图上选中的图线相对应的一束输入光束计算。点击一条图线即可选中它。当前正在处理的输入光束的标签显示在特殊点选项卡中，也显示在 :ref:`光标 <cursor>` 面板中。

  .. image:: img/func_caustic_mb_spec_points.png

绘图窗口的附加命令
----------------------------------

该函数具有与 :ref:`焦散 <func_caustic_mr_aux_cmds>` 函数相同的命令。

.. seeAlso::

  - :doc:`plot_window`
  - :doc:`plot_opers`
  - :doc:`func_caustic`
  - :doc:`func_caustic_mr`
