Pumps Window
============

:menuselection:`Window --> Pumps`

The window shows a list of different input beams for SP schema.

When you change the schema :doc:`trip-type<trip_type>` to SP, the first pump is automatically created with type :ref:`Waist <pump_mode_waist>`. Then you can add more input beams of :doc:`different types <pump_mode>` using the command :menuselection:`Pumps --> Create (Ctrl+Ins)`. |rezonator| can store an arbitrary number of input beams; however, it can calculate only one of them at time, which is called the *active pump*. Use the command  :menuselection:`Pumps --> Activate (Ctrl+Return)` to make the pump active that is marked by the green lamp in the column 'On'. All functions recalculate using active pump parameters.

If you close the window, the pumps are not lost. If you change schema trip type to SP or RR, the pumps are not lost, too, though they become useless.

  .. image:: img/pumps_window.png

|para|

When the window is closed, the parameters of active pump can be quickly accessed via double-clicking at the Pump icon in the status bar:

  .. image:: img/setup_input_beam.png 

|para|

.. seealso::

    :doc:`input_beam`, :doc:`pump_mode`
