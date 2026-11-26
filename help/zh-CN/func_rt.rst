.. index:: single: round-trip matrix (function)

往返矩阵
=================

:menuSelection:`Functions --> Round-Trip Matrix`

该函数计算光路的 :doc:`往返矩阵<round_trip_matrix>` 并将其显示在一个特殊窗口中。在元件列表中选中的元件被视为参考元件。元件矩阵的乘法从参考元件开始。

    .. image:: img/func_rt_1.png

如果光路是一个谐振腔（而不是单程系统），那么还会计算光路的 :ref:`稳定性参数<stability_param>`。

上方一行按矩阵相乘的顺序显示元件列表。你可以点击每个元件的标签，打开一个单独的 :ref:`信息窗口 <func_matrix>`，其中显示该元件的矩阵。

此外，如果启用了该 :ref:`应用设置 <app_settings_show_py_code>`，该窗口可以使用与 :doc:`func_matrix` 函数为此目的显示的相同变量名，将往返公式显示为 Python 代码。

附加命令
-------------------

除了 :ref:`标准按钮 <info_window>` 之外，该函数还增加了几个新选项。


显示所有元件矩阵
~~~~~~~~~~~~~~~~~~~~~~~~~

可以选择将所有元件的矩阵直接显示在窗口中，使用工具栏按钮：

    .. image:: img/func_rt_2.png


设置参考平面偏移
~~~~~~~~~~~~~~~~~~~~~~~~~~~

默认情况下，整个元件矩阵用于往返计算。参考平面放置在所选元件的正后方，矩阵乘法从该元件开始。但如果选择了具有长度参数的元件作为参考（例如晶体），则可以设置元件内部的一个可选偏移。此时该元件会根据 :ref:`示意图<calc_round_trip_subrange>` 被拆分为两个子元件，并计算元件内部的往返矩阵。

    .. image:: img/func_rt_3.png

按照一般的 :ref:`往返规则<propagation_dir>`，光束从左向右传播，而矩阵则从右向左相乘，因此该偏移是从元件的左边缘量起的。

对话框中的 “Axial length” 值显示最大可能的偏移，它是光束在元件中能走过的最长几何光程。对于某些元件（例如 :doc:`matrix/ElemBrewsterPlate`），轴向长度大于元件长度，因为光轴与测量长度的方向之间存在一个角度。

.. seeAlso::

    - :doc:`info_window`
    - :doc:`func_rt_plot`
    - :doc:`round_trip_matrix`
    - :doc:`func_freeze`
