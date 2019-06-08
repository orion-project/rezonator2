.. index:: single: thin lens
.. index:: single: lens (thin)

Thin Lens
=========

The thin lens has no parameter of thickness, index of refraction of glass not taken into account, and beam propagation inside of lens body not computed. 

The lens can be collecting or diverging (positive or negative) depending on the sign of focal range. 

The lens has the same focal ranges `F` for both tangential and sagittal planes. Matrices for both planes are also equal when the beam falls on the lens at the normal angle. When incident beam inclined at angle `α`, then matrices for T and S planes differ. 

The thin lens acts in the same way as a :doc:`spherical mirror <ElemCurveMirror>`, except that you have to specify the focal range instead of the radius of curvature. 

    .. image:: ElemThinLens.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
    