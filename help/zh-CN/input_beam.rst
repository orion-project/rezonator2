.. index:: single: input beam

输入光束
==========

:menuSelection:`File --> Setup Input Beam (F9)`

:menuSelection:`Windows --> Pumps --> Pump --> Edit (Enter)`

该对话框允许为 SP 光路设置输入光束参数。输入光束也称为*泵浦*。

当你将光路的 :doc:`往返类型<trip_type>` 改为 SP 时，会自动创建第一个类型为 :ref:`束腰 <pump_mode_waist>` 的泵浦。然后你可以使用 :doc:`Pumps window <pumps_window>` 添加更多 :doc:`不同类型 <pump_mode>` 的输入光束，并在它们之间切换。|rezonator| 可以存储任意数量的输入光束；但它一次只能计算其中一个，这个被称为*活动泵浦*。

该命令也可以通过双击状态栏中的泵浦类型图标，为活动泵浦调用。在 Pumps 窗口中，双击一行泵浦时会显示该对话框。

  .. image:: img/setup_input_beam.png

.. seeAlso::

  - :doc:`pump_mode`
  - :doc:`pumps_window`
