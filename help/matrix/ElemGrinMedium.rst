.. index:: single: GRIN medium

GRIN Medium
===========

GRIN medium has a gradual variation of refractive index that produces lensing effect. The element has similar matrices for both work planes, but gradient constants can be set independently for tangential and sagittal planes.

Unlike :doc:`ElemGrinLens`, this element doesn't imply any interfaces. To get proper results, you should put appropriate interface elements at both sides of the medium.


    .. image:: ElemGrinMedium.png
    
   
.. seealso::

    :doc:`./ElemGrinLens`,
    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Gradient-index optics in Wikipedia <https://en.wikipedia.org/wiki/Gradient-index_optics>`_
