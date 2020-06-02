.. index:: single: gaussian duct (medium)

Gaussian duct (medium)
======================

The element is implemented according to Siegman book "Lasers", chapter 20.3 "Gaussian apertures and complex ABCD matrices".

A gaussian duct is a transversely inhomogeneous medium in which the refractive index and the absorption coefficient may both vary transversely.

The coefficient `α` is the loss factor per unit length.

The element has the same matrices as the :doc:`ElemGrinMedium` has, with the only difference that the parameter `γ` is a complex value.

    .. image:: ElemGaussDuctMedium.png
    
.. seealso::

    :doc:`ElemGaussDuctSlab`, :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
