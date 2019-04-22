.. index:: single: flat mirror
.. index:: single: mirror (flat)

Flat Mirror
===========

Flat mirror has a unity matrix and does not affect propagating beam anyhow. But it is an important element for :ref:`round-trip matrix calculation <round_trip_matrix>` when it is placed at the end of the resonator.

    .. image:: ElemFlatMirror.png
    
Flat mirror changes its representation on the :doc:`layout <layout>` when it is located not at an end of the resonator and thus similar to :doc:`ElemPoint` can be used as helper element e.g. as a separator between two adjacent range elements having different semantics.

    .. image:: ElemFlatMirror_example.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
    