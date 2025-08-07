.. _func_caustic_mr:
.. index:: single: multirange caustic (function)

Multirange Caustic
==================

:menuSelection:`Functions --> Multirange Caustic`

The function is just like the function :doc:`func_caustic` but allows you to plot the caustic along several elements at the same time.

Parameters
----------

Elements
~~~~~~~~

A list of elements along which the caustic should be plotted. Only elements having the parameter Length are presented in the list. These are empty ranges, crystals, etc. The length parameter is usually called :param:`L`. There are buttons at the right of the list allowing you quickly select or deselect elements.

Plot accuracy
~~~~~~~~~~~~~

Fields allow you to set a number of points in the resulting graph. A points count can be input directly in the “Number of points” field. Instead, it is possible to specify a distance between points (plotting step) in the field “With step”.

The “Plottable length” value in the dialog shows the longest geometrical path a beam can travel in the element. This becomes a plot’s horizontal range. For some elements (e.g., :doc:`matrix/ElemBrewsterPlate`) the plottable length is greater than the element length because there is an angle between the optical axis and the direction the length is measured along.

Use these settings for all elements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the same number of points for each element. When you do not want to set different point numbers for elements, you just have to set this flag and specify the desired number of points only once. In this case, the same value of the parameter “Number of points” or “With step” will be used for all the elements.

  .. image:: img/func_caustic_mr_params.png

Additional Parameters
---------------------

:menuSelection:`View --> Function Params`

The function has the same plotting modes as the :ref:`caustic <func_caustic_aux_params>` function has.

Special Points
--------------

:menuSelection:`View --> Special Points`

The function calculates the same values as the :ref:`caustic <func_caustic_spec_points>` function does, but for each element separately.

.. _func_caustic_mr_aux_cmds:

Additional Commands of Plot Window
----------------------------------

The function has the same commands as the :ref:`caustic <func_caustic_aux_cmds>` function has and adds some new commands.

.. _func_caustic_mr_elem_edges:

Toggle Element Edge Markers
~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`View --> Element Edge Markers`

The command toggles the visibility of vertical lines that are drawn between neighbor plotting ranges (elements).

  .. image:: img/func_caustic_mr_elem_edges.png

Element Bound Markers Format
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Format --> Element Edge Markers Format`

The command changes the appearance of element edge markers for the current function window. A default appearance which is applied to all newly opened function windows, is controlled via :ref:`application settings <app_settings_lines>`.

.. seeAlso::
  
  :doc:`plot_window`, :doc:`plot_opers`, :doc:`func_caustic`, :doc:`func_caustic_mb`
