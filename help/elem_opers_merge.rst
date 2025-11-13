.. _elem_opers_merge:
.. index:: single: merge element (command)

Merge
=====

:menuSelection:`Element --> Merge...`

The command does the opposite of what the :doc:`elem_opers_split` command does. It merges two neighboring elements having the Length parameter into one. Actually, the command removes the second of the selected elements, and the length of the remaining one is increased by the length of the removed one.

    .. image:: img/elem_opers_merge_drawing.png

    .. note::
        The command changes the length of the remaining element. So it can not be executed when the length is not directly editable because it's controlled by a :ref:`global parameter <params_window>`.

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_split`
    - :doc:`elem_opers_insert_into`
