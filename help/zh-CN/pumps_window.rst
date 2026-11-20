.. _pumps_window:
.. index:: single: pump

泵浦窗口
============

:menuSelection:`Window --> Pumps`

该窗口显示 SP 光路的不同输入光束列表。输入光束也称为 *泵浦*。

当你把光路的 :doc:`往返类型 <trip_type>` 改为 SP 时，会自动创建第一个类型为 :ref:`Waist <pump_mode_waist>` 的泵浦，然后你可以添加更多。|rezonator| 可以保存任意数量的输入光束；但是，它一次只能计算其中一个，这个被称为 *活动泵浦*。如果你关闭该窗口，泵浦不会丢失。如果你把光路的往返类型改为 SP 或 RR，泵浦也不会丢失，只是会变得无用。

  .. image:: img/pumps_window.png

当窗口关闭时，可以通过菜单命令 :menuSelection:`File --> Setup Input Beam` 快速访问活动泵浦的参数，或者双击状态栏中的 Pump 图标：

  .. image:: img/setup_input_beam.png

命令
--------

.. --------------------------------------------------------------------------

Create
~~~~~~

:menuSelection:`Pump --> Create (Ctrl+Ins)`

添加一个 :doc:`不同类型 <pump_mode>` 的新输入光束。类型以后不能更改。如果 :ref:`应用程序设置 <app_settings_gen_labels_pump>` 允许，会为新泵浦自动生成标签。

  .. image:: img/select_pump_mode.png

.. --------------------------------------------------------------------------

Clone
~~~~~

:menuSelection:`Pump --> Clone`

该命令复制当前选中的泵浦。如果 :ref:`应用程序设置 <app_settings_gen_labels_pump>` 允许，会为新泵浦自动生成标签。

.. --------------------------------------------------------------------------

Edit
~~~~

:menuSelection:`Pump --> Edit (Return)`

该命令打开一个对话框，用于更改选中的 :ref:`泵浦参数 <pump_mode>`。输入光束的类型不能更改。

.. --------------------------------------------------------------------------

Activate
~~~~~~~~

:menuSelection:`Pump --> Activate (Ctrl+Return)`

|rezonator| 可以保存任意数量的输入光束；但是，它一次只能计算其中一个，这个被称为 *活动泵浦*。活动泵浦在 “On” 列中用绿灯标记。所有函数都会使用活动泵浦的参数重新计算。

例外的是 :doc:`func_caustic_mb` 函数，它同时处理所有输入光束。

.. --------------------------------------------------------------------------

Set Color
~~~~~~~~~

.. _pump_window_set_color:

:menuSelection:`Pump --> Set Color`

该命令允许更改选中泵浦的颜色。颜色仅对 :doc:`func_caustic_mb` 函数重要；在那里它可以用来区分图形线条。颜色的更改会立即应用到所有已打开的函数窗口。

.. --------------------------------------------------------------------------

Delete
~~~~~~

:menuSelection:`Pump --> Delete (Ctrl+Del)`

该命令删除当前选中的泵浦。如果要删除的泵浦是活动泵浦，则改为激活前一个泵浦。如果只有一个泵浦，则不能删除它。

.. --------------------------------------------------------------------------

.. seeAlso::

    - :doc:`input_beam`
    - :doc:`pump_mode`
