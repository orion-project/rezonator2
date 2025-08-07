.. _func_caustic:
.. index:: single: caustic (function)

Caustic
=======

:menuSelection:`Functions --> Caustic`

The function plots beam radius and wavefront curvature radius along specified element.

Parameters
----------

    .. image:: img/func_caustic_params.png

Element
~~~~~~~

The selector allows you to specify an element along which the caustic should be plotted. Only elements having the length parameter can be selected. These are empty ranges, crystals, etc. The length parameter is usually called :param:`L`.

Plot accuracy
~~~~~~~~~~~~~

Fields allow you to set a number of points in the resulting graph. A points count can be input directly in the “Number of points” field. Instead, it is possible to specify a distance between points (plotting step) in the field “With step”.

The “Plottable length” value in the dialog shows the longest geometrical path a beam can travel in the element. This becomes a plot’s horizontal range. For some elements (e.g., :doc:`matrix/ElemBrewsterPlate`) the plottable length is greater than the element length because there is an angle between the optical axis and the direction the length is measured along.

Additional Parameters
---------------------

.. _func_caustic_aux_params:

:menuSelection:`View --> Function Params`

In the tab you can select what should be plotted - beam radius or wavefront curvature radius. The plot remembers different units of measurement for each plotting mode.

    .. image:: img/func_caustic_aux_params.png

If there is a waist inside the element, the graph of beam radius usually requires fewer points than the wavefront curvature graph to be accurate. It is due to wavefront curvature radius changes quickly in the near-field zone and can not be approximated with a few points as it can be in the far-field zone.

    .. image:: img/func_caustic_r_1.png

    .. image:: img/func_caustic_r_2.png

.. _func_caustic_spec_points:

Special Points
--------------

:menuSelection:`View --> Special Points`

The function calculates the beam radius at the beginning and end of the plotting range. In addition, if there is a waist in the plot, its position and radius are also displayed. See the yellow-highlighted values. Beam’s Rayleigh distance (`z`\ :sub:`0`) and half of the spread angle (`V`\ :sub:`S`) in radians and degrees are also calculated based on the founded waist size.

  .. image:: img/func_caustic_spec_points.png


.. _func_caustic_aux_cmds:

Additional Commands of Plot Window
----------------------------------

Show Beam Shape
~~~~~~~~~~~~~~~

:menuSelection:`View --> Beam Shape`

The function can schematically render the beam cross section at the :ref:`cursor <cursor>` position to visualize beam astigmatism.

The beam shape panel can be moved over the plot by the mouse. It can also be resized when you click the mouse near its border shown as a thin dotted line when the mouse is above the panel.

The beam shape image can be copied separately from the whole plot; use its context menu.

  .. image:: img/func_caustic_shape.png

.. seeAlso::

    :doc:`plot_window`, :doc:`plot_opers`, :doc:`func_caustic_mr`, :doc:`func_caustic_mb`
