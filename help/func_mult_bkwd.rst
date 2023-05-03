Multiply Selected From Last To First
====================================

:menuselection:`Functions --> Multiply Selected From Last To First`

The function calculates the product of the selected elements' matrices in opposite order, from the last selected element to the first one, in the opposite ordered elements numbered in the table. See the window *2* on the picture below.

For elements whose matrices are different for forward and back beam propagation (e.g. :doc:`matrix/ElemThickLens` or :doc:`matrix/ElemSphericalInterface`) it is possible to choose what matrix should be taken. Use the red arrow button on the toolbar for specifying the forward propagation matrix, and the green arrow button for opting to the back propagation matrix.

This function is for debug and test purposes mostly so it doesn't care about the real beam propagation in chosen elements or if the selected order is physically correct.

    .. image:: img/func_mult_matrs.png

.. seealso::

    :doc:`func_mult_fwd`, :doc:`func_rt`
