Table Symbols
=============

There are cases when several values of the same parameter correspond to one element in the table.

  .. image:: img/table_symbols_1.png

For example:

* The beam radius in the tangential plane and radius of wavefront curvature in both planes are different for points lying at the crystal’s edge inside and outside the medium. 

* Beam parameters for each side and for the middle of the crystal (`z = 0`, `z = L/2` and `z = L` where `z` is the coordinate along the optical axis) are calculated separately. 

* The radius of wavefront curvature can be different for beams incident to a curve mirror or a lens and for the reflected or refracted beam.

Certain elements have more than one corresponding row in the data table for these cases. Every such row is marked with a special icon.

  .. image:: img/table_symbols_crystal.png

  .. image:: img/table_symbols_mirror.png

When you invoke the :ref:`Copy <table_opers_copy>` command via the :menuSelection:`Edit` menu or context menu of the table, the content of selected cells is copied into the clipboard as text. Row icons are also copied in the text form.

  .. image:: img/table_symbols_copy.png

.. seeAlso::

    :doc:`table_window`
