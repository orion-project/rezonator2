绘图的往返
===================

:menuSelection:`Plot --> Show Round-trip`

这是 :doc:`func_rt` 函数的一个特殊版本，它显示用于计算绘图函数某个特定值的往返过程。它取最后的 :ref:`光标 <cursor>` 位置作为计算目标，因此你可以检查得到光标行中显示的值时使用了哪些矩阵。

    .. image:: img/func_rt_plot_1.png

.. note::
    当你移动光标线时，该函数不会自动更新。在重新定位光标线后，请点击往返窗口中的 Update 按钮或按 F5。

:doc:`func_caustic_mr` 函数使用几个内部的 :doc:`func_caustic` 函数来绘制整个图。因此当你将光标线移到另一个元件时，需要打开一个单独的往返窗口。

    .. image:: img/func_rt_plot_2.png

.. TODO: it's too difficult to understand, rephrase: If the same element is plotted in several window, then displayed offset in the round-trip window can differ from a position of the cursor line in a window the round-trip is opened for. This round-trip function aims mostly debug and test purposes so it doesn't monitor very carefully from where elements are modified. Its intended usage is "move cursor, hit update". Also don't keep too many of such windows opened simultaneously in order not to get confused.

.. TODO:    .. image:: img/func_rt_plot_3.png

.. seeAlso::

    - :doc:`func_rt`
