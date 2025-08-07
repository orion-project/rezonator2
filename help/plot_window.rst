.. _plot_window:
.. index:: single: plot window

Plot Window
===========

Results of calculations of most functions are represented as graphs in a special plot window. The name of the function is shown in the window's title bar and on its button on the Windows panel of the project window. For example, “Caustic”, “Stability map”. Windows for the same function are incrementally numbered.

  .. image:: img/plot_window.png

Window Components
-----------------

Plot (1)
~~~~~~~~

It is the main part of the plot window showing graphs. It is can show various parts: coordinate axes, coordinate grids, legend, title, cursor, stability boundary lines (for :doc:`stability map <func_stabmap>`), element edge lines (for :doc:`multicaustic <func_caustic_mr>`). Click a plot part to select it. The selected part is highlighted with a color and thicker line and has its own context menu. Also, the values of the selected graph are shown in the data table.

.. --------------------------------------------------------------------------

.. _plot_window_data_table:

Data Table (2)
~~~~~~~~~~~~~~

:menuSelection:`View --> Data Table`

The table demonstrates a bulk of values that was used to plot the selected graph. The table can be useful if you need to copy a part of values; use the table's context menu. The table is not editable. Precision of displayed values is controlled by the :ref:`application settings <app_settings_format_precision>`.

.. --------------------------------------------------------------------------

Additional Parameters (3)
~~~~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`View --> Function Params`

Some functions, like :doc:`func_caustic`, have an additional set of parameters controlling what should be displayed on the plot. These parameters are described in a particular function documentation in the section "Additional Parameters".

.. --------------------------------------------------------------------------

.. _plot_window_spec_points:

Special Points (4)
~~~~~~~~~~~~~~~~~~

:menuSelection:`View --> Special Points`

The panel displays a set of values that are somehow notable for the current function. Special points are calculated automatically when this panel is visible and the function is recalculated. What is displayed in the panel is described in a particular function documentation in the section "Special Points". Precision of displayed values is controlled by the :ref:`application settings <app_settings_format_precision>`.

.. --------------------------------------------------------------------------

Cursor (5)
~~~~~~~~~~

:menuSelection:`View --> Cursor`

The cursor is one or two crossing lines that can be dragged by the mouse. The cursor panel shows coordinates of the cross point of the cursor and the function values (in T and S planes) calculated at the point under the cursor. See :doc:`plot_cursor`.

.. --------------------------------------------------------------------------

Status Bar (6)
~~~~~~~~~~~~~~

The status bar displays: 

- Units of measurement for X and Y axes. Right-click the label to change units. Or use the context menu of the respective axis.
- Number of points in the selected graph. Click a graph on the plot to select it.
- Additional information or context messages depending on a particular function type. 

.. seeAlso::

  :doc:`plot_opers`, :doc:`table_window`, :doc:`info_window`
