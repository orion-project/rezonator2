.. index:: single: Matrix3 (Python)

Class: Matrix3
==============

Extended 3×3 ray transfer matrix for misalignment calculations.

Constructor
-----------

Parameters:

- ``matrix`` (:doc:`Matrix <py_matrix>`, optional) - 2×2 ABCD matrix to convert

Example:

.. code-block:: python

    Matrix3()          # Identity matrix
    Matrix3(matrix)    # From 2×2 ABCD matrix

Properties
----------

``A``, ``B``, ``E`` (float or complex, read/write)
    First row elements

``C``, ``D``, ``F`` (float or complex, read/write)
    Second row elements

``G``, ``H``, ``I`` (float or complex, read/write)
    Third row elements

Indexing
--------

Matrix3 elements can be accessed by index (0-8):

.. code-block:: python

    m = Z.Matrix3()
    m[0] = 1  # A
    m[4] = 2  # D
    # etc.

Indexing also allows for unpacking a matrix object into separate variables:

.. code-block:: python

    m = Z.Matrix3()
    A, B, C, D, E, F, G, H, I = m

Operations
----------

Matrix3 supports multiplication with other Matrix3 objects:

.. code-block:: python

    m1 = Z.Matrix3()
    m2 = Z.Matrix3()
    result = m1 * m2
    m1 *= m2  # In-place multiplication

Example
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
