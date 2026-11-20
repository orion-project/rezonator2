.. index:: single: stability map (function)

稳定性图
=============

:menuSelection:`Functions --> Stability Map`

该函数绘制光路的稳定性参数随单个变量变化的函数 P(X)，其中 X 是光路中某个元件的参数值。

该函数仅适用于 :ref:`谐振腔 <trip_type>`，因为稳定性的概念不适用于单程系统。

参数
----------

  .. image:: img/func_stabmap_params.png

元件、参数
~~~~~~~~~~~~~~~~~~

要变化的一个元件及其参数，或一个 :ref:`全局参数 <params_window>`。该参数的值将标在稳定性图的横轴上。

变化范围
~~~~~~~~~

这些字段允许指定所选参数的起始值和结束值（绘图范围）以及结果图中的点数。点数可以直接在 “Number of points” 字段中输入。或者，也可以在 “With step” 字段中指定点之间的距离（绘图步长）。


附加参数
---------------------

:menuSelection:`View --> Function Params`

  .. image:: img/func_stabmap_stab_param.png

该选项卡定义了稳定性参数的计算方法。

- `P = (A + D) / 2`

- `P = 1 — ((A + D) / 2)²`

其中 A 和 D 是系统 :ref:`往返矩阵 <round_trip>` 的分量。在第一种情况下，当参数值在 -1 到 1 的范围内时系统稳定。在第二种情况下，当参数值在 0 到 1 的范围内时系统稳定。使用 :menuSelection:`Limits --> Y-Axis -> Stability Range` 菜单命令或函数窗口工具栏上的相应按钮，可以将图直观地适配到这些范围。

特殊点
--------------

:menuSelection:`View --> Special Points`

  .. image:: img/func_stabmap_spec_points.png

该函数的特殊点是稳定性边界。它们是变量参数的阈值，在这些阈值处光路仍保持稳定。在这些阈值处，稳定性参数取 (-1 和 1) 或 (0 和 1)，取决于计算模式（见上文）。如果光路有多个稳定性区间（见 “v_empty” 示例），则所有区间都会在选项卡中显示为 *N-beg* - *N-end*。

这些值以图的 X 轴的计量单位显示。当单位改变时，特殊点会重新计算。

绘图窗口的附加命令
----------------------------------

.. _func_stabmap_stab_lines:

切换稳定性边界标记
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`View --> Stability Boundary Markers`

该命令切换图上稳定性边界标记的可见性。它们是位于 (-1 和 1) 或 (0 和 1) 水平处的两条水平线，取决于稳定性参数的计算模式（见上文）。图线位于边界线之间时变量参数的取值决定了稳定性区间。

稳定性边界标记格式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Stability Boundary Markers Format...`

该命令更改当前函数窗口的稳定性边界标记的外观。应用于所有新打开的函数窗口的默认外观，通过 :ref:`应用设置 <app_settings_lines>` 控制。

使 Y 轴适配稳定性范围
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Limits --> Y-Axis -> Stability Range`

该命令调整纵轴的范围，使其与稳定性标记线的位置相对应。

.. seeAlso::

  - :doc:`plot_window`
  - :doc:`plot_opers`
  - :doc:`func_stabmap_2d`
