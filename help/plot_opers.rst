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

Copy Plot Format
~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Paste Plot Format
~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Format
-----------

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Limits
-----------

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu Plot
---------

.. --------------------------------------------------------------------------

Update
~~~~~~

.. --------------------------------------------------------------------------

Update With Params
~~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Freeze
~~~~~~

.. --------------------------------------------------------------------------

TS-Flipped Mode
~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Show T-Plane
~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Show S-Plane
~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Title Text
~~~~~~~~~~

.. --------------------------------------------------------------------------

X-Axis Text
~~~~~~~~~~~

.. --------------------------------------------------------------------------

Y-Axis Text
~~~~~~~~~~~

.. --------------------------------------------------------------------------

X-Axis Unit
~~~~~~~~~~~

.. --------------------------------------------------------------------------

Y-Axis Unit
~~~~~~~~~~~

.. --------------------------------------------------------------------------

Show Round-Trip
~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------
.. --------------------------------------------------------------------------

Menu View
---------

.. --------------------------------------------------------------------------

Toggle Plot Title
~~~~~~~~~~~~~~~~~

.. --------------------------------------------------------------------------

Toggle Plot Legend
~~~~~~~~~~~~~~~~~~

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
  
  :doc:`plot_window`
