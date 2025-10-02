.. _elem_opers_slide:
.. index:: single: slide element (command)

Slide
=====

:menuSelection:`Element --> Slide...`

When an element selected is located between similar elements having the Length parameter, such as :ref:`elem_empty_range` or :ref:`elem_medium_range`, the command produces an effect of shifting the element along its surrounding elements.

    .. image:: img/elem_opers_slide_drawing.png

Actually, it changes the length of both surrounding elements so that one of them becomes longer and the other one gets shorter by the same value. So that it's not necessary to select an element located between two spaces. Instead, you can select two arbitrary spaces and redistribute their lengths.

    .. note::
        The command changes the length of both elements. So it can not be executed when any of the lengths is not directly editable because it's controlled by a :ref:`global parameter <params_window>`.

A slider shows the summary length, and its knob can be dragged to redistribute the length between both elements. Alternatively, the length of an element can be edited manually; the length of the second element is recalculated accordingly.

    .. image:: img/elem_opers_slide.png

.. seeAlso::

    :doc:`elem_opers`, :doc:`elem_opers_split`, :doc:`elem_opers_insert_into`
