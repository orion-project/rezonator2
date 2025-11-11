.. index:: single: Matrix (Python)

Class: Matrix
=============

2×2 ABCD ray transfer matrix for paraxial optics.

Constructor
-----------

Parameters:

- ``A``, ``B``, ``C``, ``D`` (float or complex, optional) - Matrix elements

Example:

.. code-block:: python

    Matrix(A=1, B=0, C=0, D=1)

Properties
----------

``A``, ``B``, ``C``, ``D`` (float or complex, read/write)
    Matrix elements

Example:

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    Z.print(m.A)  # A = 1
    Z.print(m.B)  # B = 2
    Z.print(m.C)  # C = 3
    Z.print(m.D)  # D = 4

Indexing
--------

Matrix elements can be accessed by index (0-3):

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    Z.print(m[0])  # A = 1
    Z.print(m[1])  # B = 2
    Z.print(m[2])  # C = 3
    Z.print(m[3])  # D = 4

Indexing also allows for unpacking a matrix object into separate variables:

.. code-block:: python

    m = Z.Matrix(1, 2, 3, 4)
    A, B, C, D = m

Operations
----------

Multiplication
~~~~~~~~~~~~~~

Matrices can be multiplied with other matrices or :ref:`ray vectors <py_ray_vector>`:

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

Example
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

    :doc:`py_api`, :doc:`py_matrix3`, :doc:`py_ray_vector`, :ref:`Example - Basic Ray Tracing <py_example_basic_ray_tracing>`
