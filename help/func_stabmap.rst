.. index:: single: stability map (function)

Stability Map
=============

:menuSelection:`Functions --> Stability Map`

The function plots the stability parameter of the schema as a function of a single variable, P(X), where X is the parameter value of one of the schema elements.

The function works only on :ref:`resonators <trip_type>` because the idea of stability is not applicable to single-pass systems.

Parameters
----------

  .. image:: img/func_stabmap_params.png

Element, Parameter
~~~~~~~~~~~~~~~~~~

Element and its parameter to vary. The value of this parameter will be marked along the horizontal axis of the stability map plot.

Variation
~~~~~~~~~

The fields allow specifying starting and ending values of the selected parameter (plot range) and a number of points in resulting graphs. The points number can be input directly in the “Number of points” field. Instead, it is possible to specify a distance between points (plotting step) in the field “With step”.


Additional Parameters
---------------------

:menuSelection:`View --> Function Params`

  .. image:: img/func_stabmap_stab_param.png

The tab defines a method of stability parameter calculation.

- `P = (A + D) / 2`

- `P = 1 — ((A + D) / 2)²`

Where A and D are system :ref:`round-trip matrix <round_trip>` components. In the first case, the system is stable when the parameter value is in the range from -1 to 1. In the second case, the system is stable when the parameter value is in the range from 0 to 1. Use the :menuSelection:`Limits --> Y-Axis -> Stability Range` menu command or the respective button on the function's window toolbar to visually fit the plot into these ranges.

Special Points
--------------

:menuSelection:`View --> Special Points`

  .. image:: img/func_stabmap_spec_points.png

Special points of the function are stability boundaries. These are such threshold magnitudes of the variable parameter, at which the schema is still stay stable. At these thresholds, the stability parameter takes values (-1 and 1) or (0 and 1) depending on the calculation mode (see above). If the schema has several stability intervals (see the "v_empty" example), then all of them are shown in the tab as *N-beg* - *N-end*.

Values are displayed in the units of measurements of the plot's X-axis. When those are changed, the special points are recalculated.

Additional Commands of Plot Window
----------------------------------

Toggle Stability Boundary Markers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. _func_stabmap_stab_lines:

:menuSelection:`View --> Stability Boundary Markers`

The command toggles the visibility of stability boundary markers on the plot. They are two horizontal lines at levels (-1 and 1) or (0 and 1), depending on the mode of the stability parameter calculation (see above). The values of the variable parameter at which graphs lie between the boundary lines determine the stability interval.

Stability Boundary Markers Format
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Stability Boundary Markers Format...`

The command changes the appearance of stability boundary markers for the current function window. A default appearance which is applied to all newly opened function windows, is controlled via :ref:`application settings <app_settings_lines>`.

Fit Y-Axis to Stability Range
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Limits --> Y-Axis -> Stability Range`

The command adjusts vertical axis limits so that they correspond to stability marker lines positions. 

.. seeAlso::
  
  :doc:`plot_window`, :doc:`func_stabmap_2d`
