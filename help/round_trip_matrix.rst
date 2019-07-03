.. index:: single: calculation of round-trip matrix
.. index:: single: matrix of round-trip
.. index:: single: round-trip

Calculation of Round-trip Matrix 
================================

The :doc:`ray-matrix <elem_matrs>` formalism supposes the elements' matrices are multiplied one-by-one in the direction opposite to the beam :ref:`propagation direction <propagation_dir>`.

General elements
----------------

Each element characterizes by a pair of ray matrices. One matrix is for the tangential (T) plane, and the other is for the sagittal (S) plane. Round-trip matrices are calculated independently for both planes.



.. _calc_round_trip_sw:

SW
^^

If the schema is the :ref:`standing wave resonator<schema_kind_sw>`, then elements' matrices are multiplied in the next way:

- from the reference element to the first element of the schema,

- from the first element to the last one,

- from the last element to an element next to the reference. 

|para|
    
    .. image:: img/round_trip_sw_1.png

Sample result (M4 is the reference element):

    M0 = M4 × M3 × M2 × M1 × M2 × M3 × M4 × M5 × M6 × M7 × M6 × M5



.. _calc_round_trip_rr:

RR
^^

If the schema is the :ref:`ring resonator<schema_kind_rr>`, then elements' matrices are multiplied in the next way: 

- from the reference element to the first element of the schema,

- from the last element to an element next to the reference.

|para|

    .. image:: img/round_trip_rr_1.png
    
Sample result (M4 is the reference element):

    M0 = M4 × M3 × M2 × M1 × M7 × M6 × M5
    

   
.. _calc_round_trip_sp:

SP
^^

If the schema is the :ref:`single-pass system<schema_kind_sp>`, then ray passes it from the first element to the last one. Therefore, matrices are multiplied in the reverse order (from the last element to the first one).

    .. image:: img/round_trip_sp_1.png
    
Sample result (M4 is the reference element):

    M0 = M4 × M3 × M2 × M1 



Elements having length
----------------------

.. |M4_1| replace::  M4\ :sub:`1`
.. |M4_2| replace::  M4\ :sub:`2`

Some functions can analyze elements over their length, :ref:`caustic <func_caustic>` functions, for example. The reference (being analyzed) element is divided into two sub-elements for such functions. The first sub-element is a part of the original element from its left edge till the current point (in which the beam size is calculated, for example). The second sub-element is a part of the original element from the current point till the right edge. Each sub-element characterizes by its ray matrix pair.



.. _calc_round_trip_sw_1:

SW
^^

If the schema is the :ref:`standing wave resonator<schema_kind_sw>`, then elements' matrices are multiplied in the next way:

- from the left reference sub-element to the first element of the schema,

- from the first element to the last one (the reference element is accounted as whole at this step),

- from the last element to the right reference sub-element.

|para|

    .. image:: img/round_trip_sw_2.png
    
Sample result (M4 is the reference element):

	M0 = |M4_1| × M3 × M2 × M1 × M2 × M3 × M4 × M5 × M6 × M7 × M6 × M5 × |M4_2|
    
    
    
.. _calc_round_trip_rr_1:

RR
^^

If the schema is the :ref:`ring resonator <schema_kind_rr>`, then elements' matrices are multiplied in the next way: 

- from the left reference element to the first element of the schema,

- from the element to the right reference sub-element.

|para|

    .. image:: img/round_trip_rr_2.png
    
Sample result (M4 is the reference element):

    M0 = |M4_1| × M3 × M2 × M1 × M7 × M6 × M5 × |M4_2|



.. _calc_round_trip_sp_1:

SP
^^

If the schema is the :ref:`single-pass system <schema_kind_sp>`, then matrices from the left reference sub-element to the first element are multiplied.

    .. image:: img/round_trip_sp_2.png
    
Sample result (M4 is the reference element): 

    M0 = |M4_1| × M3 × M2 × M1


|para|

.. seealso::

    :doc:`elem_matrs`
