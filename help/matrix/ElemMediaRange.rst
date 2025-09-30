.. _ElemMediumRange:
.. _elem_medium_range:

Space Filled With Medium
========================

This element is a distance between two reference planes in a medium with an index of refraction `n`. The element has the same matrix as :doc:`ElemEmptyRange`, but the index of refraction affects beam radius in the medium.

Unlike :doc:`ElemPlate`, this element doesn't imply any interfaces. To get proper results, you should put appropriate interface elements at both sides of the medium.

    .. image:: ElemMediaRange.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
