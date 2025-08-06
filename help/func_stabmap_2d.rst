.. index:: single: 2d stability map (function)
.. index:: single: stability map 2d (function)

2D Stability Map
================

:menuSelection:`Functions --> 2D Stability Map`

The function plots the stability parameter of the schema as a function of two variables P(X,Y), where X and Y are values of parameters of certain elements of the schema. Both parameters can belong to the same element. The resulting graph is a heatmap where different colors correspond to different values of the stability parameter.

The function works only on :ref:`resonators <trip_type>` because the idea of stability is not applicable to single-pass systems.

Parameters
----------

  .. image:: img/func_stabmap_2d_params.png


Variable 1 (X) / Variable 2 (Y)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These are elements’ parameters to be varied along the horizontal or vertical axis of the stability map respectively.

Variation
~~~~~~~~~

The fields allow specifying starting and ending values of selected parameters (plot range) and a number of points in resulting graphs. The points number can be input directly in the "Number of points" field. Instead, it is possible to specify a distance between points (plotting step) in the field "With step".

Additional Parameters
---------------------

:menuSelection:`View --> Function Params`

  .. image:: img/func_stabmap_2d_stab_param.png

The tab defines a method of stability parameter calculation.

- `P = (A + D) / 2`

- `P = 1 — ((A + D) / 2)²`

Where A and D are system :ref:`round-trip matrix <round_trip>` components. In the first case, the system is stable when the parameter value is in the range from -1 to 1. In the second case, the system is stable when the parameter value is in the range from 0 to 1. Use the :menuSelection:`Limits --> Z-Axis -> Stability Range` menu command or the respective button on the function's window toolbar to visually fit the heatmap colors into these ranges.

Special Points
--------------

The function does not calculate any special points.

Additional Commands of Plot Window
----------------------------------

Color Scale Format
~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Color Scale Format`

The command opens a dialog for changing the format of the color scale. 

Fit Z-Axis to Stability Range
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Limits --> Z-Axis -> Stability Range`

The command adjusts the color scale so that it corresponds to stability boundary values: (-1 to 1) or (0 to 1) depending on the mode of stability parameter calculation (see above).

.. seeAlso::
  
  :doc:`plot_window`, :doc:`func_stabmap`
