Round-trip for Plot
===================

:menuSelection:`Plot --> Show Round-trip`

It's a special kind of the :doc:`func_rt` function showing a round-trip used for calculation of a particular value of a plot function. It takes the last :ref:`cursor <cursor>` position as the calculation target so you can check what matrices were used for getting the values shown in the cursor line.

    .. image:: img/func_rt_plot_1.png

.. note::
    The function does not update automatically when you move the cursor line. Please click the Update button or hit F5 in the round-trip window after you reposition the cursor line.

The :doc:`func_caustic_mr` function uses several inner :doc:`func_caustic` functions for plotting the whole graph. So you need to open a separate round-trip window when you shift the cursor line to a different element.

    .. image:: img/func_rt_plot_2.png

.. TODO: it's too difficult to understand, rephrase: If the same element is plotted in several window, then displayed offset in the round-trip window can differ from a position of the cursor line in a window the round-trip is opened for. This round-trip function aims mostly debug and test purposes so it doesn't monitor very carefully from where elements are modified. Its intended usage is "move cursor, hit update". Also don't keep too many of such windows opened simultaneously in order not to get confused.

.. TODO:    .. image:: img/func_rt_plot_3.png

.. seeAlso::

    - :doc:`func_rt`
