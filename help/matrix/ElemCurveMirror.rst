.. index:: single: curved mirror
.. index:: single: spherical mirror
.. index:: single: mirror (spherical)

Spherical Mirror
================

The spherical mirror has the same radii of curvature *R* for both tangential and sagittal planes. Matrices for both planes are also equal when beam falls on mirror at the normal angle. But when incident beam inclines at angle *α*, then matrices for T and S planes differ.

A spherical mirror can be concave or convex depending on the sign of radius of curvature. The sign of ROC is not related to the beam :ref:`propagation direction <propagation_dir>` but only governs how the mirror transforms the beam's wavefront.

The spherical mirror acts exactly like :doc:`thin lens <ElemThinLens>`, except that you have to specify the radius of curvature instead of the focal range. Even on :doc:`layout <schema_layout>` it is displayed as a lens when it is not located at ends of optical system.

    .. image:: ElemCurveMirror.png
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`