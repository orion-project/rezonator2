.. _elem_opers_insert_into:
.. index:: single: insert into (command)

插入到内部
===========

:menuSelection:`Element --> Insert Into...`

该命令对任何具有 Length 参数的元件都处于活动状态，例如 :ref:`elem_empty_range` 或 :ref:`elem_brewster_crystal`。该命令打开 :doc:`catalog`，为新元件选择类型。新创建的元件会被插入到当前所选元件的中间，将其拆分为两个同类型的元件，它们的总长度等于原元件的长度。

    .. image:: img/elem_opers_split_drawing.png

该命令等同于 :doc:`elem_opers_split` 命令，但允许你选择插入到拆分后两部分之间的元件类型（:doc:`elem_opers_split` 命令只允许插入 :ref:`elem_point`），但它不允许设置拆分的比例（始终使用系数 0.5）。

    .. note::
        该命令会更改原元件的长度。因此，当长度不能直接编辑（因为它由 :ref:`全局参数 <params_window>` 控制）时，该命令无法执行。

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_split`
    - :doc:`elem_opers_merge`
