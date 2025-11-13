.. index:: single: brewster interface

Brewster Interface
==================

.. |n1| replace:: `n`\ :sub:`1`
.. |n2| replace:: `n`\ :sub:`2`

The element represents the flat boundary between two media having indexes of refraction |n1| and |n2|. The beam is considered coming at Brewster's angle at the boundary.

    .. image:: ElemBrewsterInterface.png

.. warning::

    The left and the right adjacent elements of an interface element should be either :doc:`ElemEmptyRange` or :doc:`ElemMediaRange`. |rezonator| doesn't complain if they are not, but you can get incorrect results.

.. seeAlso::

    - :doc:`ElemSphericalInterface`
    - :doc:`ElemNormalInterface`
    - :doc:`ElemTiltedInterface`
    - :doc:`../elem_matrs`
    - :doc:`../catalog`
    - :doc:`../elem_props`
    - `Brewster's angle in Wikipedia <http://en.wikipedia.org/wiki/Brewster's_angle>`_
