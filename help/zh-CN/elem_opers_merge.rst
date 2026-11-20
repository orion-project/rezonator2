.. _elem_opers_merge:
.. index:: single: merge element (command)

合并
=====

:menuSelection:`Element --> Merge...`

该命令的作用与 :doc:`elem_opers_split` 命令相反。它将两个具有 Length 参数的相邻元件合并为一个。实际上，该命令会删除所选元件中的第二个，而保留下来的那个元件的长度会增加被删除元件的长度。

    .. image:: img/elem_opers_merge_drawing.png

    .. note::
        该命令会更改保留下来的元件的长度。因此，当长度不能直接编辑（因为它由 :ref:`全局参数 <params_window>` 控制）时，该命令无法执行。

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_split`
    - :doc:`elem_opers_insert_into`
