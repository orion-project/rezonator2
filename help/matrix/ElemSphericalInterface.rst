.. index:: single: spherical interface

Spherical Interface
===================

.. |n1| replace:: `n`\ :sub:`1`
.. |n2| replace:: `n`\ :sub:`2`

The element represents the spherical boundary of curvature `R` between two media having indexes of refraction |n1| and |n2|. The beam is considered coming normally at the boundary.  

An interface can be concave or convex depending on the sign of the radius of curvature. The positive direction of the curvature is from left to right, and it corresponds to the primary beam :ref:`propagation direction <propagation_dir>` in the schema.

  .. image:: ElemSphericalInterface.png

.. note::
  Since |rezonator| 2.1 the `standard sign convention <https://en.wikipedia.org/wiki/Radius_of_curvature_(optics)>`_ is used for the interface ROC. Older files are converted automatically when opened. In the older app versions, you will not be able to load files saved in 2.1 or paste elements via Clipboard. 

.. warning::
  The left and the right adjacent elements of an interface element should be either :doc:`ElemEmptyRange` or :doc:`ElemMediaRange`. |rezonator| doesn't complain if they are not, but you can get incorrect results.
    
.. seeAlso::

    :doc:`ElemNormalInterface`, :doc:`ElemBrewsterInterface`, :doc:`ElemTiltedInterface`, :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
