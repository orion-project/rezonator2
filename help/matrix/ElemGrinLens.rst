.. index:: single: GRIN lens
.. index:: single: lens (GRIN)

GRIN Lens
=========

GRIN lens is a plane-parallel slab of material with a gradual variation of refractive index that produces lensing effect. The element has similar matrices for both work planes, but gradient constants can be set independently for tangential and sagittal planes.

Note that this element represents a piece of GRIN-medium in free space. It has planar input and output interfaces. If you want to build a more complex GRIN-rod, e.g., having one or both curved interfaces, you should use the :doc:`ElemGrinMedium` element and surround it with appropriate interface elements.

Beam propagation inside the lens also can be calculated.

Full element
    
    .. image:: ElemGrinLens.png
    
Left half-pass

    .. image:: ElemGrinLens_left.png
    
Right half-pass

    .. image:: ElemGrinLens_right.png
    
.. seealso::

    :doc:`./ElemGrinMedium`,
    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Gradient-index optics in Wikipedia <https://en.wikipedia.org/wiki/Gradient-index_optics>`_
