.. index:: single: intermode beats frequency (function)
.. index:: single: repetition rate

Intermode Beats Frequency
=========================

:menuselection:`Functions --> Intermode Beats Frequency` 

The function computes the intermode beats frequency of the resonator.

For :ref:`standing wave system<schema_kind_sw>` (SW):

    .. image:: img/func_reprate_sw.png

For :ref:`ring resonator<schema_kind_rr>` (RR):

    .. image:: img/func_reprate_rr.png

Where `c` is the speed of light, and `L` is calculated as the  sum of lengths of all elements having parameter :elem_param:`Length`  

List of elements participating in total resonator length is shown in function's window.

Value of refractive index is given for elements where its magnitude is not unity. For example, a Brewster plate having thickness 4mm and refractive index 1.5 will be marked like

    **0.0048074** × 1.5 *(Cr)*
    
Here 0.0048074 is the length of the path of light in the plate (in meters). It differs from the value of :elem_param:`Length` parameter (4mm in this case), because of length is measured along plate's axis Z, but light travels under some angle to that.

    .. image:: img/func_reprate_window.png
    
.. seealso::

    :doc:`info_window`
