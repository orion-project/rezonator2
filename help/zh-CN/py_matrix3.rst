.. index:: single: Matrix3 (Python)

类：Matrix3
==============

用于 :ref:`misalignment <func_misalign>` 计算的扩展 3×3 光线传输矩阵。

构造函数
-----------

参数：

- ``matrix`` (:doc:`Matrix <py_matrix>`, optional) - 要转换的 2×2 ABCD 矩阵

示例：

.. code-block:: python

    Matrix3()          # Identity matrix
    Matrix3(matrix)    # From 2×2 ABCD matrix

属性
----------

``A``, ``B``, ``E`` (float or complex, read/write)
    第一行元素

``C``, ``D``, ``F`` (float or complex, read/write)
    第二行元素

``G``, ``H``, ``I`` (float or complex, read/write)
    第三行元素

索引
--------

可以通过索引（0-8）访问 Matrix3 元素：

.. code-block:: python

    m = Z.Matrix3()
    m[0] = 1  # A
    m[4] = 2  # D
    # etc.

索引还允许将一个矩阵对象解包为单独的变量：

.. code-block:: python

    m = Z.Matrix3()
    A, B, E, C, D, F, G, H, I = m

运算
----------

Matrix3 支持与其它 Matrix3 对象相乘：

.. code-block:: python

    m1 = Z.Matrix3()
    m2 = Z.Matrix3()
    result = m1 * m2
    m1 *= m2  # In-place multiplication

示例
-------

.. code-block:: python

    import rezonator as Z

    # Convert ABCD matrix to 3x3
    abcd = Z.Matrix(1, 0.1, 0, 1)
    m3 = Z.Matrix3(abcd)

    # Identity matrix
    identity = Z.Matrix3()
    Z.print(identity)

.. seeAlso::

  - :doc:`py_api`
  - :doc:`py_matrix`
  - :doc:`py_ray_vector`
  - :doc:`func_misalign`
