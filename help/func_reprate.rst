.. index:: single: intermode beat frequency (function)
.. index:: single: repetition rate (function)

Repetition Rate
===============

:menuSelection:`Functions --> Repetition Rate`

The function computes the intermode beat frequency of the resonator.

For :ref:`standing wave system<schema_kind_sw>` (SW):

    .. image:: img/func_reprate_sw.png

For :ref:`ring resonator<schema_kind_rr>` (RR):

    .. image:: img/func_reprate_rr.png

Where `c` is the speed of light, and `L` is calculated as the  sum of the lengths of all elements having parameter :elem_param:`Length`

The list of elements participating in the total resonator length is shown in the function's window.

The value of the refractive index is given for elements where its magnitude is not unity. For example, a Brewster plate having a thickness of 4 mm and a refractive index of 1.5 will be marked like this:

    **0.0048074** × 1.5 *(Cr)*

Here 0.0048074 is the length of the path of light in the plate (in meters). It differs from the value of the :elem_param:`Length` parameter (4 mm in this case), because of length is measured along plate’s axis Z, but light travels under some angle to that.

    .. image:: img/func_reprate_window.png

.. seeAlso::

    - :doc:`info_window`
