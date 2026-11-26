.. _func_caustic_mr:
.. index:: single: multirange caustic (function)

多程段焦散
==================

:menuSelection:`Functions --> Multirange Caustic`

该函数与 :doc:`func_caustic` 函数基本相同，但允许你同时沿多个元件绘制焦散。

参数
----------

元件
~~~~~~~~

要沿其绘制焦散的元件列表。列表中只呈现具有 Length 参数的元件。这些元件是空程段、晶体等。长度参数通常称为 :param:`L`。列表右侧有按钮，允许你快速选择或取消选择元件。

绘图精度
~~~~~~~~~~~~~

这些字段允许你设置结果图中的点数。点数可以直接在 “Number of points” 字段中输入。或者，也可以在 “With step” 字段中指定点之间的距离（绘图步长）。

对话框中的 “Plottable length” 值显示光束在元件中能走过的最长 :ref:`几何光程 <axial_length>`。这成为图的横向范围。对于某些元件（例如 :doc:`matrix/ElemBrewsterPlate`），可绘制长度大于元件长度，因为光轴与测量长度的方向之间存在一个角度。

对所有元件使用这些设置
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

对每个元件使用相同的点数。当你不想为各元件设置不同的点数时，只需设置此标志并只指定一次所需的点数即可。在这种情况下，所有元件都将使用参数 “Number of points” 或 “With step” 的相同值。

  .. image:: img/func_caustic_mr_params.png

附加参数
---------------------

:menuSelection:`View --> Function Params`

该函数具有与 :ref:`焦散 <func_caustic_aux_params>` 函数相同的绘制模式。

特殊点
--------------

:menuSelection:`View --> Special Points`

该函数计算与 :ref:`焦散 <func_caustic_spec_points>` 函数相同的值，但对每个元件分别计算。

.. _func_caustic_mr_aux_cmds:

绘图窗口的附加命令
----------------------------------

该函数具有与 :ref:`焦散 <func_caustic_aux_cmds>` 函数相同的命令，并增加了一些新命令。

.. _func_caustic_mr_elem_edges:

切换元件边缘标记
~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`View --> Element Edge Markers`

该命令切换在相邻绘制范围（元件）之间绘制的竖线的可见性。

  .. image:: img/func_caustic_mr_elem_edges.png

元件边界标记格式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Element Edge Markers Format`

该命令更改当前函数窗口的元件边缘标记的外观。应用于所有新打开的函数窗口的默认外观，通过 :ref:`应用设置 <app_settings_lines>` 控制。

.. seeAlso::

  - :doc:`plot_window`
  - :doc:`plot_opers`
  - :doc:`func_caustic`
  - :doc:`func_caustic_mb`
