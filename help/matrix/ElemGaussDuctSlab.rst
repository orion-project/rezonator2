.. index:: single: gaussian duct (slab)

Gaussian duct (stab)
====================

The element is implemented according to Siegman book "Lasers", chapter 20.3 "Gaussian apertures and complex ABCD matrices".

A gaussian duct is a transversely inhomogeneous medium in which the refractive index and the absorption coefficient may both vary transversely.

The coefficient `α` is the loss factor per unit length.

The element has the same matrices as the :doc:`ElemGrinLens` has, with the only difference that the parameter `γ` is a complex value.

    .. image:: ElemGaussDuctSlab.png
    
.. seealso::

    :doc:`ElemGaussDuctMedium`, :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
