.. index:: single: RayVector (Python)
.. _py_ray_vector:

类：RayVector
================

表示近轴光学中的 :ref:`ray vector <ray_vector>`（Y, V）。

构造函数
-----------

参数：

- ``Y`` (float, optional) - 光线位置（m）
- ``V`` (float, optional) - 光线角度（rad）

示例：

.. code-block:: python

    RayVector(Y=0, V=0)

属性
----------

``Y`` (float, read/write)
    光线位置坐标

``V`` (float, read/write)
    光线角度坐标

索引
--------

可以通过索引访问矢量分量：

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    Z.print(v[0])  # Y = 0.001
    Z.print(v[1])  # V = 0.01

索引还允许将一个光线矢量对象解包为单独的变量：

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    Y, V = v

运算
----------

加法
~~~~~~~~

.. code-block:: python

    v1 = Z.RayVector(0.001, 0.01)
    v2 = Z.RayVector(0.002, 0.02)
    v_sum = v1 + v2
    v1 += v2  # In-place addition

矩阵变换
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

    v = Z.RayVector(0.001, 0.01)
    m = Z.Matrix(1, 0.1, 0, 1)
    v_out = m * v

示例
-------

.. code-block:: python

    import rezonator as Z

    # Create input ray
    ray_in = Z.RayVector(Y=0.001, V=0.01)

    # Transform through optical system
    lens = Z.Matrix(1, 0, -10, 1)
    ray_out = lens * ray_in

    Z.print('Input ray:', ray_in)
    Z.print('Output ray:', ray_out)

.. seeAlso::

  - :doc:`py_api`
  - :doc:`py_matrix`
  - :doc:`py_matrix3`
  - :ref:`Example - Basic Ray Tracing <py_example_basic_ray_tracing>`
