.. index:: single: gaussian lens
.. index:: single: lens (gaussian)

Gaussian aperture with thin lens
================================

The element is implemented according to Siegman book "Lasers", chapter 20.3 "Gaussian apertures and complex ABCD matrices".

This a combination of a :doc:`ElemThinLens` or a :doc:`ElemCurveMirror` with :doc:`ElemGaussAperture`.

The coefficient `α` is the total loss factor through an aperture.

    .. image:: ElemGaussApertureLens.png
    
.. seealso::

    :doc:`ElemGaussAperture`, :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
    