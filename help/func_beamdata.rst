Beam Parameters at Elements
===========================

:menuSelection:`Functions --> Beam Parameters at Elements`

The function computes the beam parameters at each element of the schema. A collection of calculated values is displayed in a table. The first column of the table indicates the element.

  .. image:: img/func_beamdata.png

Columns
-------

Beam radius (Wt × Ws)
~~~~~~~~~~~~~~~~~~~~~

The beam radius at the element. The displayed measurement units can be changed via the context menu of the column header or by the "Units of measurement" toolbar button.

Aperture ratio (At × As)
~~~~~~~~~~~~~~~~~~~~~~~~

The ratio of the element's aperture to the beam size at the element: `A = (D/2) / w` where `D` is the element's aperture diameter.

Wavefront ROC (Rt × Rs)
~~~~~~~~~~~~~~~~~~~~~~~

The radius of wavefront curvature at the element. The displayed measurement units can be changed via the context menu of the column header or by the "Units of measurement" toolbar button.

Half div. angle (Vt × Vs)
~~~~~~~~~~~~~~~~~~~~~~~~~

Half of the spread angle in the far-field zone. The displayed measurement units can be changed via the context menu of the column header or by the "Units of measurement" toolbar button.

.. note::
  There may be several rows in the table corresponding to the same element. These rows are marked with different icons. See :doc:`wnd_table_symbols` for details.

.. seeAlso::
  :doc:`wnd_table`, :doc:`func_caustic`
