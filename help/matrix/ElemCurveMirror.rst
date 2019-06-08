.. index:: single: curved mirror
.. index:: single: spherical mirror
.. index:: single: mirror (spherical)

Spherical Mirror
================

The spherical mirror has the same radii of curvature `R` for both tangential and sagittal planes. Matrices for both planes are equal too when the beam falls on the mirror at the normal angle. When incident beam inclined at angle `α`, then matrices for T and S planes differ. 

A spherical mirror can be concave or convex depending on the sign of radius of curvature. The sign of ROC is not related to the beam :ref:`propagation direction <propagation_dir>` but only governs how the mirror transforms the beam’s wavefront. 

The spherical mirror acts in the same way as a :doc:`thin lens <ElemThinLens>`, except that you have to specify the radius of curvature instead of the focal range. Even on the :doc:`layout <schema_layout>`, it is displayed as a lens when it does not locate at ends of the optical system. 

    .. image:: ElemCurveMirror.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`