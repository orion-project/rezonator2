.. index:: single: multibeam caustic (function)

Multibeam Caustic
=================

:menuSelection:`Functions --> Multibeam Caustic`

This is a special version of the :doc:`func_caustic_mr` for :ref:`single-pass <schema_kind_sp>` systems. It calculates beam radius for all input beams that are set up in the :doc:`pumps_window`.

Use the command :ref:`Set Color <pump_window_set_color>` in the Pumps window to make an unique appearance for a graph line corresponding to some input beam. Color changes are applied immediately to all opened function windows.

  .. image:: img/func_caustic_mb_colors.png

Parameters
----------

The function has the same parameters as the :doc:`func_caustic_mr` function has.

Additional Parameters
---------------------

The function has no additional parameters

Special Points
--------------

:menuSelection:`View --> Special Points`

The function calculates the same values as the :ref:`caustic <func_caustic_spec_points>` function does, but for each element separately. Values are calculated only of one input beam corresponding to a graph that is currently selected on the plot. Click a graph line to select it. 

  .. image:: img/func_caustic_mb_spec_points.png

Additional Commands of Plot Window
----------------------------------

The function has the same commands as the :ref:`caustic <func_caustic_mr_aux_cmds>` function has.

.. seeAlso::
  
  :doc:`plot_window`, :doc:`func_caustic`, :doc:`func_caustic_mr`
