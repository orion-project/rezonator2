.. _elem_opers_replace:
.. index:: single: replace element (command)

Replace
=======

:menuSelection:`Element --> Replace...`

The command opens the :doc:`catalog` to choose a type for the currently selected element. The current element gets replaced with an element of the new type, and its properties are copied to the new element. Parameters are copied by name, so, for example, if you replace a :doc:`matrix/ElemCurveMirror` with a :doc:`matrix/ElemThinLens`, the angle will be copied because both of them have the parameter :param:`α`. While the focal range will not be copied because they are named differently and the application doesn't include any heuristics to say the :param:`R` and :param:`F` mean almost the same.

    .. image:: img/catalog.png

.. seeAlso::

    - :doc:`elem_opers`
    - :doc:`elem_opers_append`
