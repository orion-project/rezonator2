.. _elem_opers_split:
.. index:: single: split element (command)

Split
=====

:menuSelection:`Element --> Split...`

The command is active for any element having the Length parameter, such as :ref:`elem_empty_range` or :ref:`elem_brewster_crystal`. It allows for splitting the element into two elements of the same type having the summary length equivalent to the length of the original element. Actually, only one new element is inserted, which takes a part of the length of the original element, and the length of the original element is reduced accordingly.

    .. image:: img/elem_opers_split_drawing.png

This command is a kind of script providing a convenient and simpler way instead of inserting a new element using the :doc:`elem_opers_append` command and adjusting parameters of both the old and new elements via the :doc:`elem_opers_props` command. Later, both elements can be edited independently. Alternatively, both elements can be changes simultaneously using the :doc:`elem_opers_slide` command.

The parameters dialog of the command allows for setting a :ref:`label <elem_props_label>` for the new element. By default, the label is generated automatically but can be changed manually. The label of the original element can also be changed if needed.

The length of the source element is shared proportionally between the original and new elements. The proportion can be changed by moving the slider or manual editing.

There is a tiny double-arrow button between label editors. The button defines if the new element should be placed before or after the original element. By default, the new element is inserted after the original one. Click the button to swap the elements.

If the "Insert point between ranges" option is checked, then a :ref:`elem_point` is automatically created between the original and new elements. The label for the point element is generated automatically and can be edited manually.

    .. image:: img/elem_opers_split.png

.. seeAlso::

    :doc:`elem_opers`, :doc:`elem_opers_insert_into`, :doc:`elem_opers_merge`, :doc:`elem_opers_slide`