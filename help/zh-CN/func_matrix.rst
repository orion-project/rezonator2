.. _func_matrix:

元件矩阵
================

:menuSelection:`Element --> Show Matrix`

:menuSelection:`Element --> Show All Matrices`

  .. image:: img/func_matrix.png

该窗口显示在 :doc:`schema_elems` 上选中的一个或多个元件的光线矩阵。

如果一个元件对于正向和反向 :ref:`传播 <propagation_dir>` 具有不同的矩阵，例如 :doc:`matrix/ElemThickLens` 就是这样，那么两个矩阵都会显示。

除了便于查看的表格视图外，该窗口还可以将矩阵显示为 Python NumPy 变量。此功能默认不启用，应在 :ref:`应用设置 <app_settings_show_py_code>` 中启用。这是一个可用于测试脚本的调试功能。它与 :doc:`func_rt` 函数配合使用，后者也可以使用相同的变量名显示用于往返乘法的 Python 代码。

.. seeAlso::

    - :doc:`info_window`
