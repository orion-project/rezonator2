.. _elem_opers_slide:
.. index:: single: slide element (command)

滑动
=====

:menuSelection:`Element --> Slide...`

当所选元件位于具有 Length 参数的同类元件之间时，例如 :ref:`elem_empty_range` 或 :ref:`elem_medium_range`，该命令会产生沿其周围元件移动该元件的效果。

    .. image:: img/elem_opers_slide_drawing.png

实际上，它会改变两个周围元件的长度，使得其中一个变长，另一个变短相同的量。因此，不必选择位于两个间隔之间的元件。相反，你可以选择任意两个间隔，并在它们之间重新分配长度。

    .. note::
        该命令会改变两个元件的长度。因此，当其中任何一个长度不能直接编辑（因为它由 :ref:`全局参数 <params_window>` 控制）时，该命令无法执行。

一个滑块显示总长度，它的旋钮可以拖动，以在两个元件之间重新分配长度。或者，也可以手动编辑一个元件的长度；第二个元件的长度会据此重新计算。

    .. image:: img/elem_opers_slide.png

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_split`
    - :doc:`elem_opers_insert_into`
