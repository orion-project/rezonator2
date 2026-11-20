.. _elem_opers_split:
.. index:: single: split element (command)

拆分
=====

:menuSelection:`Element --> Split...`

该命令对任何具有 Length 参数的元件都处于活动状态，例如 :ref:`elem_empty_range` 或 :ref:`elem_brewster_crystal`。它允许将元件拆分为两个同类型的元件，它们的总长度等于原元件的长度。实际上，只插入一个新元件，它取走原元件的一部分长度，原元件的长度则相应减少。

    .. image:: img/elem_opers_split_drawing.png

这个命令是一种脚本，提供了一种方便而更简单的方式，取代了使用 :doc:`elem_opers_append` 命令插入新元件，再通过 :doc:`elem_opers_props` 命令调整新旧两个元件参数的做法。之后，两个元件可以独立编辑。或者，也可以使用 :doc:`elem_opers_slide` 命令同时更改两个元件。

    .. note::
        该命令会更改原元件的长度。因此，当长度不能直接编辑（因为它由 :ref:`全局参数 <params_window>` 控制）时，该命令无法执行。

该命令的参数对话框允许为新元件设置一个 :ref:`标签 <elem_props_label>`。默认情况下，标签会自动生成，但也可以手动更改。如有需要，原元件的标签也可以更改。

源元件的长度按比例在原元件和新元件之间分配。可以通过移动滑块或手动编辑来更改比例。

标签编辑器之间有一个小小的双箭头按钮。该按钮定义新元件应该放在原元件之前还是之后。默认情况下，新元件插入到原元件之后。点击该按钮可交换两个元件。

如果选中 “Insert point between ranges” 选项，则会在原元件和新元件之间自动创建一个 :ref:`elem_point`。该点元件的标签会自动生成，也可以手动编辑。

    .. image:: img/elem_opers_split.png

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_insert_into`
    - :doc:`elem_opers_merge`
    - :doc:`elem_opers_slide`
