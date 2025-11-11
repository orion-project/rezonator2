.. _ElemPoint:
.. _elem_point:
.. index:: single: point element

Point Element
=============

The point element is a helper element having a unity matrix. It does not affect propagating beam anyhow. The element differs from :doc:`ElemFlatMirror` only in visual representation but the same in calculations.

    .. image:: ../../img/elem/ElemPoint.svg

For example, one can use this element as a separator between two adjacent range elements with different semantic and then change the value of ranges independently.

    .. image:: ElemPoint_example.png

.. seeAlso::

    - :doc:`../elem_matrs`
    - :doc:`../catalog`
    - :doc:`../elem_props`
