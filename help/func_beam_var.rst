.. index:: single: beamsize variation (function)

Beamsize Variation
==================

:menuSelection:`Functions --> Beamsize Variation`

The function plots beam radius at a specified element as a function of a parameter of one of the schema elements.

Parameters
----------

  .. image:: img/func_beam_var_params.png

Element, Parameter
~~~~~~~~~~~~~~~~~~

Element and its parameter to vary. The value of this parameter will be marked along the horizontal axis of the stability map plot.

Variation
~~~~~~~~~

The fields allow specifying starting and ending values of the selected parameter (plot range) and a number of points in resulting graphs. The points number can be input directly in the “Number of points” field. Instead, it is possible to specify a distance between points (plotting step) in the field “With step”.

Plot positions
~~~~~~~~~~~~~~

An element to calculate beam radius at. If the element has length (a crystal, for example) you can also specify an offset from the left edge of the element to calculate beam radius within it. If you set the offset value larger than the element's length it will be truncated to the length. 

Additional Parameters
---------------------

The function has no additional parameters

Special Points
--------------

The function does not calculate any special points.

Additional Commands of Plot Window
----------------------------------

Show Beam Shape
~~~~~~~~~~~~~~~

:menuSelection:`View --> Beam Shape`

The function can schematically render the beam cross section at the :ref:`cursor <cursor>` position to visualize beam astigmatism.

The beam shape panel can be moved over the plot by the mouse. It can also be resized when you click the mouse near its border shown as a thin dotted line when the mouse is above the panel.

The beam shape image can be copied separately from the whole plot; use its context menu.

  .. image:: img/func_beam_var_shape.png

.. seeAlso::
  
  :doc:`plot_window`, :doc:`func_stabmap`, :doc:`func_caustic`
