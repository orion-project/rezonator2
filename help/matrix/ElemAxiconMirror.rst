.. index:: single: axicon mirror
.. index:: single: mirror (axicon)

Axicon Mirror
=============

Axicon mirror is a specialized type of mirror which has a conical surface; it can be converging or diverging depending on the sign of its angle `θ`. 

The axicon mirror has the same matrices for both planes when the beam falls on the mirror at the normal angle. When incident beam inclined at angle `α`, then matrices for T and S planes differ. 

Because the ray matrix depends on the radius of the incident beam, this element can be used only in single-pass schemas where a beam radius is known from the beginning. In resonators, it operates as a plane or a point having a unity matrix.

The axicon mirror acts in the same way as :doc:`axicon lens <ElemAxiconLens>`, except that you don't have to specify an index of refraction. Even on the :doc:`layout <schema_layout>`, it is displayed as a lens when not located at the ends of the optical system. 

    .. image:: ElemAxiconMirror.png
    
.. seealso::

    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Axicon in Wikipedia <https://en.wikipedia.org/wiki/Axicon>`_
