.. index:: single: 2d stability map (function)
.. index:: single: stability map 2d (function)

二维稳定性图
================

:menuSelection:`Functions --> 2D Stability Map`

该函数绘制光路的稳定性参数随两个变量变化的函数 P(X,Y)，其中 X 和 Y 是光路中某些元件的参数值。两个参数可以属于同一个元件。结果图是一幅热力图，不同的颜色对应稳定性参数的不同值。

该函数仅适用于 :ref:`谐振腔 <trip_type>`，因为稳定性的概念不适用于单程系统。

参数
----------

  .. image:: img/func_stabmap_2d_params.png


变量 1 (X) / 变量 2 (Y)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

这些是要分别沿稳定性图的横轴或纵轴变化的元件参数或 :ref:`全局参数 <params_window>`。

变化范围
~~~~~~~~~

这些字段允许指定所选参数的起始值和结束值（绘图范围）以及结果图中的点数。点数可以直接在 “Number of points” 字段中输入。或者，也可以在 “With step” 字段中指定点之间的距离（绘图步长）。

附加参数
---------------------

:menuSelection:`View --> Function Params`

  .. image:: img/func_stabmap_2d_stab_param.png

该选项卡定义了稳定性参数的计算方法。

- `P = (A + D) / 2`

- `P = 1 — ((A + D) / 2)²`

其中 A 和 D 是系统 :ref:`往返矩阵 <round_trip>` 的分量。在第一种情况下，当参数值在 -1 到 1 的范围内时系统稳定。在第二种情况下，当参数值在 0 到 1 的范围内时系统稳定。使用 :menuSelection:`Limits --> Z-Axis -> Stability Range` 菜单命令或函数窗口工具栏上的相应按钮，可以将热力图的颜色直观地适配到这些范围。

特殊点
--------------

该函数不计算任何特殊点。

绘图窗口的附加命令
----------------------------------

颜色标尺格式
~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Color Scale Format`

该命令打开一个对话框，用于更改颜色标尺的格式。

使 Z 轴适配稳定性范围
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Limits --> Z-Axis -> Stability Range`

该命令调整颜色标尺，使其与稳定性边界值相对应：(-1 到 1) 或 (0 到 1)，取决于稳定性参数的计算模式（见上文）。

.. seeAlso::

  - :doc:`plot_window`
  - :doc:`plot_opers`
  - :doc:`func_stabmap`
