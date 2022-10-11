.. index:: single: thick lens
.. index:: single: lens (thick)

Thick Lens
==========

.. |R1| replace:: `R`\ :sub:`1`
.. |R2| replace:: `R`\ :sub:`2`

Thick lens element can be parameterized with length `L` and index of refraction `n`. Also, it has two radii of curvature |R1| and |R2| for its left and right surfaces.

Lens surface's ROC can be concave or convex depending on the sign of the radius of curvature. The positive direction of the curvature is from left to right, and it corresponds to the primary beam :ref:`propagation direction <propagation_dir>` in the schema.

Full element
    
    .. image:: ElemThickLens.png
    
Left half-pass

    .. image:: ElemThickLens_left.png
    
Right half-pass

    .. image:: ElemThickLens_right.png

.. note::
    Note that a sign convention applied here contradicts the `standard sign convention <https://en.wikipedia.org/wiki/Radius_of_curvature_(optics)>`_ for optical surfaces. This is for historical reasons and will be changed to match standards. Compare that the :doc:`../calc_lens` tool already uses the conventional approach.

.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
