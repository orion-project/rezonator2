.. _trip_type:
.. index:: single: round-trip type

Round-Trip Type
===============

:menuSelection:`File --> Change Trip Type`

The command also can be invoked by double-clicking on the round-trip type icon in the status bar.

  .. image:: img/round_trip_dlg.png


.. _schema_kind_sw:
.. index:: single: standing wave system

SW, Standing wave system
------------------------
 
A product of elements' matrices is calculated over system double pass (round-trip); a beam passes through each element twice (excluding boundary elements). 

  .. image:: img/round_trip_sw.png


.. _schema_kind_rr:
.. index:: single: ring resonator

RR, Ring resonator 
------------------

A beam passes through each element once. After the last element (rightmost), the beam comes into the first one (leftmost) again. 

  .. image:: img/round_trip_rr.png


.. _schema_kind_sp:
.. index:: single: single-pass system

SP, Single-pass system 
----------------------

In this case, a beam comes into the first schema element (leftmost) and goes out from the last one (rightmost). 

  .. image:: img/round_trip_sp.png

To set the input beam parameters, you can use the command :doc:`Setup Input Beam <input_beam>`.

|para|

.. seeAlso::

    :doc:`round_trip_matrix`