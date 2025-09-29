.. _elem_opers_insert_into:
.. index:: single: insert into (command)

Insert Into
===========

:menuSelection:`Element --> Insert Into...`

The command is active for any element having the Length parameter, such as :ref:`elem_empty_range` or :ref:`elem_brewster_crystal`. The command opens the :doc:`catalog` to choose a type for a new element. The newly created element gets inserted into the middle of the currently selected element, splitting it into two elements of the same type having the summary length equivalent to the length of the original element.

The command is equivalent to the :doc:`elem_opers_split` command but allows you to select an element type that is inserted between split parts (the :doc:`elem_opers_split` command only allows you to insert :ref:`elem_point`), but it does not allow you to set the proportion of splitting (it always uses the factor 0.5).

    .. image:: img/elem_opers_split_drawing.png

.. seeAlso::

    :doc:`elem_opers`, :doc:`elem_opers_split`, :doc:`elem_opers_merge`