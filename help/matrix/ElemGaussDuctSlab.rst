.. index:: single: gaussian duct (slab)

Gaussian duct (stab)
====================

The element is implemented according to Siegman book "Lasers" (1986, ISBN 0-935702-11-5), chapter 20.3 "Gaussian apertures and complex ABCD matrices".

A gaussian duct is a transversely inhomogeneous medium in which the refractive index and the absorption coefficient may both vary transversely.

The coefficient `α` is the loss factor per unit length.

The element has the same matrices as the :doc:`ElemGrinLens` has, with the only difference that the parameter `γ` is a complex value (see page 791 formula 43 in the Siegman book).

    .. image:: ElemGaussDuctSlab.png

.. seeAlso::

    - :doc:`ElemGaussDuctMedium`
    - :doc:`../elem_matrs`
    - :doc:`../catalog`
    - :doc:`../elem_props`
