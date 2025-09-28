.. _elem_opers_split_range:

Split Range
===========

:menuSelection:`Element --> Split Range...`

:menuSelection:`Context menu --> Split Range...`

The command is active for any element having the Length parameter, such as :ref:`elem_empty_range` or :ref:`elem_brewster_crystal`. It allows for splitting the element into two elements of the same type having the summary length equivalent to the length of the original element. Actually, only one new element is inserted, which takes a part of the length of the original element, and the length of the original element is reduced accordingly.

    .. image:: img/elem_opers_split_range_drawing.png

This command is a kind of script providing a convenient and simpler way instead of inserting a new element using the :doc:`elem_opers_append` command and adjusting parameters of both the old and new elements via the :doc:`elem_opers_props` command. Later, both elements can be edited independently.

The parameters dialog of the command allows for setting a :ref:`label <elem_props_label>` for the new element. By default, the label is generated automatically but can be changed manually. The label of the original element can also be changed if needed.

The length of the source element is shared proportionally between the original and new elements. The proportion can be changed by moving the slider or manual editing.

There is a tiny double-arrow button between label editors. The button defines if the new element should be placed before or after the original element. By default, the new element is inserted after the original one. Click the button to swap the elements.

    .. image:: img/elem_opers_split_range.png

.. seeAlso::

    :doc:`elem_opers`