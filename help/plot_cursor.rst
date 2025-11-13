.. _cursor:
.. index:: single: cursor
.. index:: single: plot cursor

Plot Cursor
===========

:menuSelection:`View --> Cursor`

The cursor is one or two crossing lines that can be dragged by the mouse. The cursor panel shows coordinates of the cross point of the cursor and the function values (in T and S planes) calculated at the point under the cursor. Precision of displayed values is controlled by the :ref:`application settings <app_settings_format_precision>`.

The function values under the cursor are calculated accurately, so they are not interpolated by the existing points in a graph. An exception is :ref:`frozen <func_freeze>` function where functions values under the cursor are calculated by interpolation between adjacent points of graphs.

Cursor lines could be undesired when you :ref:`copy a plot <plot_opers_copy_image>` as a image into the clipboard. Instead of turning them on and off manually, there is the :ref:`application option <app_settings_export_hide_cursor>` that does it automatically.

  .. image:: img/plot_cursor.png

Type
----

:menuSelection:`View --> Cursor --> Vertical Line`

:menuSelection:`View --> Cursor --> Horizontal Line`

:menuSelection:`View --> Cursor --> Both Lines`

You can quickly toggle visibility of the cursor using the button :menuSelection:`Cursor` at the left of the cursor panel. Cursor type can be a vertical line, a horizontal line, or both lines. You can choose the desired type in a dropdown menu under the cursor toggling button.

  .. image:: img/plot_cursor_dropdown.png

Positioning
-----------

Besides of dragging it by the mouse, you can precisely set the position of the cursor. To do it, double-click X or Y coordinates in the cursor panel, type the desired value in the dialog, and press the Enter key.

  .. image:: img/plot_cursor_set_pos.png

Follow Mouse
~~~~~~~~~~~~

:menuSelection:`View --> Cursor --> Follow Mouse (F7)`

Besides of the menu option, there is also the respective button at the right of the cursor panel. When the option is toggled on, the cursor moves instantly following the mouse, and the function values are recalculated immediately. Double-click to stop the following.

Appearance
----------

:menuSelection:`Format --> Cursor Lines Format...`

The default appearance of the cursor lines is governed by :ref:`program preferences <app_settings_lines>`. It’s applied to all newly opened plot windows and to those windows where the cursor format has not been overridden. The default appearance can be overridden by the menu command and stored in the schema project file.

.. seeAlso::

  - :doc:`plot_window`
