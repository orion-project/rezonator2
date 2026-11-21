.. _round_trip:
.. index:: single: calculation of round-trip matrix
.. index:: single: matrix of round-trip
.. index:: single: round-trip

往返矩阵的计算
================================

:doc:`光线矩阵 <elem_matrs>` 方法假设元件的矩阵沿与光束 :ref:`传播方向 <propagation_dir>` 相反的方向逐个相乘。

一般元件
----------------

每个元件由一对光线矩阵刻画。一个矩阵用于切向 (T) 平面，另一个用于弧矢 (S) 平面。往返矩阵对两个平面分别独立计算。



.. _calc_round_trip_sw:

SW
^^

如果光路是 :ref:`驻波谐振腔<schema_kind_sw>`，则元件矩阵按以下方式相乘：

- 从参考元件到光路的第一个元件，

- 从第一个元件到最后一个元件，

- 从最后一个元件到参考元件旁边的元件。

|para|

    .. image:: img/round_trip_sw_1.png

示例结果（M4 是参考元件）：

    M0 = M4 × M3 × M2 × M1 × M2 × M3 × M4 × M5 × M6 × M7 × M6 × M5



.. _calc_round_trip_rr:

RR
^^

如果光路是 :ref:`环形谐振腔<schema_kind_rr>`，则元件矩阵按以下方式相乘：

- 从参考元件到光路的第一个元件，

- 从最后一个元件到参考元件旁边的元件。

|para|

    .. image:: img/round_trip_rr_1.png

示例结果（M4 是参考元件）：

    M0 = M4 × M3 × M2 × M1 × M7 × M6 × M5



.. _calc_round_trip_sp:

SP
^^

如果光路是 :ref:`单程系统<schema_kind_sp>`，则光线从第一个元件到最后一个元件通过它。因此，矩阵按相反顺序相乘（从最后一个元件到第一个元件）。

    .. image:: img/round_trip_sp_1.png

示例结果（M4 是参考元件）：

    M0 = M4 × M3 × M2 × M1


.. _calc_round_trip_subrange:

具有长度的元件
----------------------

.. |M4_1| replace::  M4\ :sub:`1`
.. |M4_2| replace::  M4\ :sub:`2`

某些函数可以沿元件的长度分析元件，例如 :ref:`焦散 <func_caustic>` 函数。对于这类函数，参考（被分析的）元件被分成两个子元件。第一个子元件是原元件从其左边缘到当前点（例如在该点计算光束半径）的部分。第二个子元件是原元件从当前点到右边缘的部分。每个子元件都由其光线矩阵对刻画。



.. _calc_round_trip_sw_1:

SW
^^

如果光路是 :ref:`驻波谐振腔<schema_kind_sw>`，则元件矩阵按以下方式相乘：

- 从左侧参考子元件到光路的第一个元件，

- 从第一个元件到最后一个元件（在此步骤中参考元件按整体计入），

- 从最后一个元件到右侧参考子元件。

|para|

    .. image:: img/round_trip_sw_2.png

示例结果（M4 是参考元件）：

	M0 = |M4_1| × M3 × M2 × M1 × M2 × M3 × M4 × M5 × M6 × M7 × M6 × M5 × |M4_2|



.. _calc_round_trip_rr_1:

RR
^^

如果光路是 :ref:`环形谐振腔 <schema_kind_rr>`，则元件矩阵按以下方式相乘：

- 从左侧参考元件到光路的第一个元件，

- 从该元件到右侧参考子元件。

|para|

    .. image:: img/round_trip_rr_2.png

示例结果（M4 是参考元件）：

    M0 = |M4_1| × M3 × M2 × M1 × M7 × M6 × M5 × |M4_2|



.. _calc_round_trip_sp_1:

SP
^^

如果光路是 :ref:`单程系统 <schema_kind_sp>`，则将从左侧参考子元件到第一个元件的矩阵相乘。

    .. image:: img/round_trip_sp_2.png

示例结果（M4 是参考元件）：

    M0 = |M4_1| × M3 × M2 × M1


|para|

.. seeAlso::

    - :doc:`elem_matrs`
