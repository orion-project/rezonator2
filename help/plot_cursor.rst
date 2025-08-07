.. _cursor:
.. index:: single: cursor
.. index:: single: plot cursor

Plot Cursor
===========

:menuSelection:`View --> Cursor`

The cursor is one or two crossing lines that can be dragged by the mouse. The cursor panel shows coordinates of the cross point of the cursor and the function values (in T and S planes) calculated at the point under the cursor.

The function values under the cursor are calculated accurately, so they are not interpolated by the existing points in a graph. An exception is :ref:`frozen <func_freeze>` function where functions values under the cursor are calculated by interpolation between adjacent points of graphs.

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

The appearance of the cursor lines is governed by :ref:`program preferences <app_settings_lines>`.

.. seeAlso::

  :doc:`plot_window`
