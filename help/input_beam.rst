.. index:: single: input beam

Input Beam
==========

:menuSelection:`File --> Setup Input Beam (F9)`

:menuSelection:`Windows --> Pumps --> Pump --> Edit (Enter)`

The dialog allows to set up the input beam parameter for SP schemas. The input beam is also called a *pump*.

When you change the schema :doc:`trip-type<trip_type>` to SP, the first pump is automatically created with type :ref:`Waist <pump_mode_waist>`. Then you can add more input beams of :doc:`different types <pump_mode>` using the :doc:`Pumps window <pumps_window>` and switch between them. |rezonator| can store an arbitrary number of input beams; however, it can calculate only one of them at time, which is called the *active pump*.

The command also invoked for the active pump by double-clicking on the pump type icon in the status bar. In the Pumps window, the dialog is shown when you double-click a pump row.

  .. image:: img/setup_input_beam.png

.. seeAlso::

  - :doc:`pump_mode`
  - :doc:`pumps_window`
