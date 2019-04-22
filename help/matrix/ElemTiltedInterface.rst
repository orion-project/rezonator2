.. index:: single: tilted interface

Tilted Interface
================

.. |n1| replace:: *n*\ :sub:`1`
.. |n2| replace:: *n*\ :sub:`2`

The element represents the flat boundary between two media having indexes of refraction |n1| and |n2|. Then incident angle of the beam is set via element's parameter *α*.

    .. image:: ElemTiltedInterface_1.png

It is sometimes convenient to set the angle inside the |n2| media as if one would want to set *β* angle instead of *α*. But there is no additional *β* parameter in the element, as it leads to ambiguity what angle should be considered. Instead, one can just give the negative sign to the angle and it forces reZonator to treat this angle as being inside of the |n2| media and calculate the first angle accordingly.

    .. image:: ElemTiltedInterface_2.png

.. warning::

    The left and the right adjacent elements of an interface element should be either :doc:`ElemEmptyRange` or :doc:`ElemMediaRange`. reZonator doesn't complain if they are not, but you can get incorrect results.
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
