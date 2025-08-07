.. _pumps_window:
.. index:: single: pump

Pumps Window
============

:menuSelection:`Window --> Pumps`

The window shows a list of different input beams for SP schema. The input beam is also called a *pump*.

When you change the schema :doc:`trip-type<trip_type>` to SP, the first pump is automatically created with type :ref:`Waist <pump_mode_waist>`, and then you can add more. |rezonator| can store an arbitrary number of input beams; however, it can calculate only one of them at a time, which is called the *active pump*. If you close the window, the pumps are not lost. If you change the schema trip type to SP or RR, the pumps are not lost, too, though they become useless.

  .. image:: img/pumps_window.png

When the window is closed, the parameters of the active pump can be quickly accessed via the menu command :menuSelection:`File --> Setup Input Beam`, or by double-clicking at the Pump icon in the status bar:

  .. image:: img/setup_input_beam.png 

Commands
--------

.. --------------------------------------------------------------------------

Create
~~~~~~

:menuSelection:`Pump --> Create (Ctrl+Ins)`

Add a new input beam of :doc:`different types <pump_mode>`. The type can not be changed later. A label is automatically generated for the new pump if that is allowed by the :ref:`application settings <app_settings_gen_labels_pump>`.

  .. image:: img/select_pump_mode.png

.. --------------------------------------------------------------------------

Clone
~~~~~

:menuSelection:`Pump --> Clone`

The command makes a copy of the currently selected pump. A label is automatically generated for the new pump if that is allowed by the :ref:`application settings <app_settings_gen_labels_pump>`.

.. --------------------------------------------------------------------------

Edit
~~~~

:menuSelection:`Pump --> Edit (Return)`

The command opens a dialog for changing the selected :ref:`pump parameters <pump_mode>`. The type of input beam can not be changed.

.. --------------------------------------------------------------------------

Activate
~~~~~~~~

:menuSelection:`Pump --> Activate (Ctrl+Return)`

|rezonator| can store an arbitrary number of input beams; however, it can calculate only one of them at a time, which is called the *active pump*. The active pump is marked with the green lamp in the column "On". All functions recalculate using active pump parameters.

An exclusion is the :doc:`func_caustic_mb` function which processes all input beams at the same time.

.. --------------------------------------------------------------------------

Set Color
~~~~~~~~~

.. _pump_window_set_color:

:menuSelection:`Pump --> Set Color`

The command allows for changing the color of the selected pump. The color is only important for the :doc:`func_caustic_mb` function; there it allows to distinguish graph lines. Color changes are applied immediately to all opened function windows.

.. --------------------------------------------------------------------------

Delete
~~~~~~

:menuSelection:`Pump --> Delete (Ctrl+Del)`

The command deletes the currently selected pump. If the deleting pump is active, then the previous pump gets activated instead. If there is the only on pump, it can not be deleted.

.. --------------------------------------------------------------------------

.. seeAlso::

    :doc:`input_beam`, :doc:`pump_mode`
