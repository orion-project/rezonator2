Multiply Selected
=================

:menuSelection:`Functions --> Multiply Selected`

The function calculates the product of the selected elements' matrices.

By default, elements are multiplied in the order they are numbered in the table. The option "Multiply in order of selection" allows you to multiply elements in the order they were selected, not the order they appear in the scheme. This allows for different combinations of elements for multiplication.

The option "Multiply in reverse order" allows you to multiply elements in reverse order, starting with the last selected element and moving backwards to the first one.

For elements whose matrices are different for forward and back beam propagation (e.g., :doc:`matrix/ElemThickLens` or :doc:`matrix/ElemSphericalInterface`) it is possible to choose what matrix should be taken by enabling the "Use back propagation matrices" option.

By default, disabled elements are ignored during multiplication. The option "Use disabled elements" allows for including disabled elements in the multiplication process.

This function is for debug and test purposes mostly, so it doesn't care about the real beam propagation in the chosen elements or if the selected order is physically correct.

    .. image:: img/func_matrix_mult.png

.. seeAlso::

    - :doc:`func_rt`
