.. index:: single: axicon mirror
.. index:: single: mirror (axicon)

Axicon Mirror
=============

Axicon mirror is a specialized type of mirror that has a conical surface; it can be converging or diverging depending on the sign of its angle `θ`. 

The axicon mirror has the same matrices for both planes when the beam falls at the normal angle. When incident beam inclined at angle `α`, then matrices for T and S planes differ. 

Because the ray matrix depends on the radius of the incident beam `r`, this element can be used effectively only in single-pass schemas where a beam radius is known from the beginning. In resonators, it operates as a plane or a point, having a unity matrix. (It's not about the real axicon, it's just a convention of the software.)

The axicon mirror acts in the same way as :doc:`axicon lens <ElemAxiconLens>`, except that you don't have to specify an index of refraction. Even on the :ref:`layout <layout>`, it is displayed as a lens when not located at the ends of the optical system. 

    .. image:: ElemAxiconMirror.png
    
.. note::
  
  In real optical systems, axicons used to turn a Gaussian beam into a non-diffractive `Bessel-like beam <https://en.wikipedia.org/wiki/Bessel_beam>`_. But |rezonator| can't calculate such beams, so the result of calculation of schemas with axicons is only mathematically correct, but hardly can be correlated with real situations.
  
.. seeAlso::

    :doc:`../elem_matrs`,
    :doc:`../catalog`,
    :doc:`../elem_props`,
    `Axicon in Wikipedia <https://en.wikipedia.org/wiki/Axicon>`_
