.. index:: single: Matrix (Python)

类：Matrix
=============

用于近轴光学的 2×2 ABCD 光线传输矩阵。

构造函数
-----------

参数：

- ``A``, ``B``, ``C``, ``D`` (float or complex, optional) - 矩阵元素

示例：

.. code-block:: python

    Matrix(A=1, B=0, C=0, D=1)

属性
----------

``A``, ``B``, ``C``, ``D`` (float or complex, read/write)
    矩阵元素

示例：

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    Z.print(m.A)  # A = 1
    Z.print(m.B)  # B = 2
    Z.print(m.C)  # C = 3
    Z.print(m.D)  # D = 4

索引
--------

可以通过索引（0-3）访问矩阵元素：

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    Z.print(m[0])  # A = 1
    Z.print(m[1])  # B = 2
    Z.print(m[2])  # C = 3
    Z.print(m[3])  # D = 4

索引还允许将一个矩阵对象解包为单独的变量：

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    A, B, C, D = m

运算
----------

乘法
~~~~~~~~~~~~~~

矩阵可以与其它矩阵或 :ref:`ray vectors <py_ray_vector>` 相乘：

.. code-block:: python

    m1 = Z.Matrix(1, 0.1, 0, 1)
    m2 = Z.Matrix(1, 0, -10, 1)

    # Matrix multiplication
    m_total = m1 * m2

    # In-place multiplication
    m1 *= m2

    # Matrix-vector multiplication
    v_in = Z.RayVector(0.001, 0.01)
    v_out = m1 * v_in

示例
-------

.. code-block:: python

    import rezonator as Z

    # Create matrices for optical elements
    lens = Z.Matrix(1, 0, -1/0.1, 1)  # f=100mm
    space = Z.Matrix(1, 0.05, 0, 1)   # d=50mm

    # Calculate system matrix
    system = space * lens * space

    Z.print('System matrix:', system)
    Z.print('A =', system.A, 'D =', system.D)

.. seeAlso::

    - :doc:`py_api`
    - :doc:`py_matrix3`
    - :doc:`py_ray_vector`
    - :ref:`Example - Basic Ray Tracing <py_example_basic_ray_tracing>`
