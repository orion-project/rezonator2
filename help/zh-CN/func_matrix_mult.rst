所选元件矩阵相乘
=================

:menuSelection:`Functions --> Multiply Selected`

该函数计算所选元件的矩阵的乘积。

默认情况下，元件按它们在表中编号的顺序相乘。选项 “Multiply in order of selection” 允许你按元件被选中的顺序、而不是它们在光路中出现的顺序来相乘。这允许用不同的元件组合来进行乘法。

选项 “Multiply in reverse order” 允许你按相反顺序相乘元件，从最后选中的元件开始，向前回到第一个元件。

对于矩阵在光束正向和反向传播时不同的元件（例如 :doc:`matrix/ElemThickLens` 或 :doc:`matrix/ElemSphericalInterface`），可以通过启用选项 “Use back propagation matrices” 来选择应使用哪个矩阵。

默认情况下，被禁用的元件在乘法中会被忽略。选项 “Use disabled elements” 允许将禁用的元件包含到乘法过程中。

该函数主要用于调试和测试目的，因此它并不关心光束在所选元件中的真实传播情况，也不关心所选顺序在物理上是否正确。

    .. image:: img/func_matrix_mult.png

.. seeAlso::

    - :doc:`func_rt`
