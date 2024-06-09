Multiply Selected From First To Last
====================================

:menuselection:`Functions --> Multiply Selected From First To Last`

The function calculates the product of the selected elements' matrices in direct order, from the first selected element to the last one, in the order elements numbered in the table. See the window *1* on the picture below.

An order in what elements get multiplied is the order in what they were selected, not the order they are in scheme. This allows for different combination of element for multiplication.

For elements whose matrices are different for forward and back beam propagation (e.g. :doc:`matrix/ElemThickLens` or :doc:`matrix/ElemSphericalInterface`) it is possible to choose what matrix should be taken. Use the red arrow button on the toolbar for specifying the forward propagation matrix, and the green arrow button for opting to the back propagation matrix.

This function is for debug and test purposes mostly so it doesn't care about the real beam propagation in chosen elements or if the selected order is physically correct.

    .. image:: img/func_mult_matrs.png

.. seealso::

    :doc:`func_mult_bkwd`, :doc:`func_rt`
