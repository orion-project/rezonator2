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
    Since |rezonator| 2.1 the `standard sign convention <https://en.wikipedia.org/wiki/Radius_of_curvature_(optics)>`_ is used for lens surface ROC. Older files are converted automatically when opened. In the older app versions, you will not be able to load files saved in 2.1 or paste elements via Clipboard. 

.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
