.. index:: single: spherical interface

Spherical Interface
===================

.. |n1| replace:: *n*\ :sub:`1`
.. |n2| replace:: *n*\ :sub:`2`

The element represents the spherical boundary of curvature *R* between two media having indexes of refraction |n1| and |n2|. The beam is considered inciding normally at the boundary.  

An interface can be concave or convex depending on the sign of the radius of curvature. The positive direction of the curvature is from left to right, and it corresponds to the primary beam :ref:`propagation direction <propagation_dir>` in the schema.

    .. image:: ElemSphericalInterface.png
    
.. warning::

    The left and the right adjacent elements of an interface element should be either :doc:`ElemEmptyRange` or :doc:`ElemMediaRange`. reZonator doesn't complain if it is not, but you can get incorrect results.
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
