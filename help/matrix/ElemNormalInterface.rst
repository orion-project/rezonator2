.. index:: single: normal interface

Normal Interface
================

.. |n1| replace:: *n*\ :sub:`1`
.. |n2| replace:: *n*\ :sub:`2`

The element represents the flat boundary between two media having indexes of refraction |n1| and |n2|. The beam is considered inciding normally at the boundary. 

    .. image:: ElemNormalInterface.png

.. warning::

    The left and the right adjacent elements of an interface element should be either :doc:`ElemEmptyRange` or :doc:`ElemMediaRange`. reZonator doesn't complain if it is not, but you can get incorrect results.
    
.. seealso::

    :doc:`../elem_matrs`, :doc:`../catalog`, :doc:`../elem_props`
