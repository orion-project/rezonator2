.. index:: single: flat mirror
.. index:: single: mirror (flat)

Flat Mirror
===========

The flat mirror has a unity matrix and does not affect the propagating beam. It still is an essential  element for :doc:`round-trip matrix calculation <../round_trip_matrix>` when placed at the end of the resonator. 

    .. image:: ElemFlatMirror.png
    
The flat mirror changes its representation on the :doc:`layout <layout>` when it does not locate at the end of the resonator. Similar to :doc:`ElemPoint`, you can use it as a helper element, e.g., as a separator between two adjacent range elements having different semantics. 

    .. image:: ElemFlatMirror_example.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
    