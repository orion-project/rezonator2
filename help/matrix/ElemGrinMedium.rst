.. index:: single: GRIN medium

GRIN Medium
===========

.. |n2| replace:: `n`\ :sub:`2`

GRIN medium has a quadratic transverse variation of refractive index, with either a maxium or minimum on axis, that produces lensing effect. The element has similar matrices for both work planes, but gradient constants |n2| can be set independently for tangential and sagittal planes.

Unlike :doc:`ElemGrinLens`, this element doesn't imply any interfaces. To get proper results, you should put appropriate interface elements at both sides of the medium.

    .. image:: ElemGrinMedium.png
    
When the gradient constant |n2| is set to zero, the element becomes simple :doc:`ElemMediaRange`.

.. seealso::

    :doc:`./ElemGrinLens`,
    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Gradient-index optics in Wikipedia <https://en.wikipedia.org/wiki/Gradient-index_optics>`_
