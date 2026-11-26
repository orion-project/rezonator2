.. index:: single: intermode beat frequency (function)
.. index:: single: repetition rate (function)

重复频率
===============

:menuSelection:`Functions --> Repetition Rate`

该函数计算谐振腔的纵模拍频。

对于 :ref:`驻波系统<schema_kind_sw>` (SW)：

    .. image:: img/func_reprate_sw.png

对于 :ref:`环形谐振腔<schema_kind_rr>` (RR)：

    .. image:: img/func_reprate_rr.png

其中 `c` 是光速，`L` 计算为所有具有参数 :elem_param:`Length` 的元件的长度之和

参与谐振腔总长度的元件列表显示在该函数的窗口中。

对于折射率不为 1 的元件，会给出其折射率的值。例如，一块厚度为 4 mm、折射率为 1.5 的布儒斯特板会这样标记：

    **0.0048074** × 1.5 *(Cr)*

这里 0.0048074 是光在板中走过的光程长度（以米为单位）。它与 :elem_param:`Length` 参数的值（本例中为 4 mm）不同，因为长度是沿板的 Z 轴测量的，而光是以某个角度相对于该方向传播的。

    .. image:: img/func_reprate_window.png

.. seeAlso::

    - :doc:`info_window`
