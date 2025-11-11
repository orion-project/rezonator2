Plot Operations
===============

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Edit
---------

.. --------------------------------------------------------------------------

.. _plot_opers_copy_graph:

Copy Graph Data
~~~~~~~~~~~~~~~

:menuSelection:`Edit --> Copy Graph Data...`

The command opens a dialog for exporting graph data to the clipboard. You can choose what to export and the target format. It is an advanced version of the :ref:`plot_opers_copy_graph_ctx` command which is available in the graph's context menu.

.. --------------------------------------------------------------------------

.. _plot_opers_copy_image:

Copy Plot Image
~~~~~~~~~~~~~~~

:menuSelection:`Edit --> Copy Plot Image`

The command copies the current plot as an image into the clipboard. By default, the cursor lines are also copied as a part of the image, but this can be overridden by the :ref:`application option <app_settings_export_hide_cursor>`. The command is also available in the plot :ref:`context menu <plot_opers_copy_image_ctx>`.

.. --------------------------------------------------------------------------

.. TODO: Copy Plot Format
.. TODO: ~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: Paste Plot Format
.. TODO: ~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Format
-----------

The menu contains commands for customizing of various visual aspects of plot: appearance of axes, titles,  lines, etc. Some functions have its own specific commands besides of the common ones. Those commands are described in help topics for respective functions in the "Additional Commands" section.

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Limits
-----------

The menu contains commands for adjusting ranges of plot axes. Some functions have its own specific commands besides of the common ones. Those commands are described in help topics for respective functions in the "Additional Commands" section.

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Plot
---------

.. --------------------------------------------------------------------------

Update
~~~~~~

:menuSelection:`Plot --> Update (F5)`

The command calculates the function again. All graphs, special points, and cursor values are updated. The command is inaccessible if the current function is :ref:`frozen <func_freeze>`.

.. --------------------------------------------------------------------------

Update With Parameters
~~~~~~~~~~~~~~~~~~~~~~

:menuSelection:`Plot --> Update With Params (Ctrl+F5)`

The command does the same as the Update command, but before the function will be recalculated you can specify new arguments for the function. But not all arguments are available for changing. Particularly, you can not change variable parameter of the function. But it's possible to change the number of points in the obtained graphs or their limits.

.. --------------------------------------------------------------------------

Freeze
~~~~~~

:menuSelection:`Plot --> Freeze (Ctrl+F)`

The command freezes the current plot window. See :doc:`func_freeze` for details.

.. --------------------------------------------------------------------------

TS-Flipped Mode
~~~~~~~~~~~~~~~

:menuSelection:`Plot --> TS-Flipped Mode`

If the option is on, then the S-graph is shown vertically mirrored.

  .. image:: img/plot_opers_flip_ts.png

.. --------------------------------------------------------------------------

Show T-Plane
~~~~~~~~~~~~

:menuSelection:`Plot --> Show T-plane`

The command toggles visibility of graphs for the tangential (T) plane.

  .. note::
    One of the graphs (for T or S plane) always stays visible. It means if one of the graphs is hidden and you try to hide the other one, then the first becomes visible.

.. --------------------------------------------------------------------------

Show S-Plane
~~~~~~~~~~~~

:menuSelection:`Plot --> Show S-plane`

The command toggles visibility of graphs for the sagittal (S) plane.

  .. note::
    One of the graphs (for T or S plane) always stays visible. It means if one of the graphs is hidden and you try to hide the other one, then the first becomes visible.

.. --------------------------------------------------------------------------

.. TODO: Title Text
.. TODO: ~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: X-Axis Text
.. TODO: ~~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: Y-Axis Text
.. TODO: ~~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: X-Axis Unit
.. TODO: ~~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: Y-Axis Unit
.. TODO: ~~~~~~~~~~~

.. --------------------------------------------------------------------------

Show Round-Trip
~~~~~~~~~~~~~~~

The command shows a list of elements in the sequence their matrices have been multiplied when the round-trip for the current function is built. See :doc:`func_rt_plot`.

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

.. TODO: Menu View
.. TODO: ---------

.. --------------------------------------------------------------------------

.. TODO: Toggle Plot Title
.. TODO: ~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

.. TODO: Toggle Plot Legend
.. TODO: ~~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Context Menu
------------

.. --------------------------------------------------------------------------

.. _plot_opers_copy_image_ctx:

Copy Plot Image
~~~~~~~~~~~~~~~

:menuSelection:`Context menu --> Copy Plot Image`

See :ref:`plot_opers_copy_image`.

.. --------------------------------------------------------------------------

.. _plot_opers_copy_graph_ctx:

Copy Graph Data
~~~~~~~~~~~~~~~

:menuSelection:`Context menu --> Copy Graph Data`

:menuSelection:`Context menu --> Copy Graph Data (this segment)`

:menuSelection:`Context menu --> Copy Graph Data (all segments)`

The command exports graph values into the clipboard using default :ref:`export settings <app_settings_export_opts>`. For functions like :doc:`func_caustic_mr`, which split graphs into segments, it's possible to copy all segments as one data block or only the currently selected segment. There is an :ref:`advanced version <plot_opers_copy_graph>` of this command allowing you to control all export settings via a dialog.

.. --------------------------------------------------------------------------

.. seeAlso::

  - :doc:`plot_window`
